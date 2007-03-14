/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QUrl>
#include <QTreeWidget>

#include "packagelist.h"
#include "database.h"
#include "downloader.h"

PackageList::PackageList()
        : QObject()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    m_root = ".";
    m_configFile = "/packages.txt";
	m_curSite = 0;

}

PackageList::~PackageList()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
}

bool PackageList::hasConfig()
{
    return QFile::exists(m_root + m_configFile);
}

void PackageList::addPackage(const Package &package)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << package.toString();
#endif

    m_packageList.append(new Package(package));
}

Package *PackageList::getPackage(const QString &pkgName, const QByteArray &version)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QList<Package*>::iterator it = m_packageList.begin();
    for ( ; it != m_packageList.end(); ++it)
        if ((*it)->name() == pkgName) {
            if(!version.isEmpty() && (*it)->version() != version)
                continue;
            return (*it);
        }
    return NULL;
}

void PackageList::listPackages(const QString &title)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    qDebug() << title;
    QList<Package*>::iterator it;
    for (it = m_packageList.begin(); it != m_packageList.end(); ++it)
        qDebug() << (*it)->toString(true," - ");
}

bool PackageList::writeToFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    if (m_packageList.count() == 0)
        return false;

    QString fileName = _fileName.isEmpty() ? m_root + m_configFile : _fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "could not open '"  << fileName << "' for writing (" << file.errorString() << ")";
        return false;
    }

    // this needs to be enhanced to fit current Package content
    // also maybe move this into class Package -> Package::write(QTextStream &out)
    QTextStream out(&file);
    out << "# package list" << "\n";
    out << "@format 1.0" << "\n";
    QList<Package*>::iterator it;
    for (it = m_packageList.begin(); it != m_packageList.end(); ++it)
        (*it)->write(out);
    return true;
}

bool PackageList::readFromFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString fileName = _fileName.isEmpty() ? m_root + m_configFile : _fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;

    m_packageList.clear();
    QTextStream in(&file);

    QString comment = in.readLine();
    QString format = in.readLine();
		
    while (!in.atEnd())
    {
        Package pkg;
        if (pkg.read(in))
        {
            if ( pkg.isInstalled(Package::BIN) || m_curSite && !m_curSite->isExclude(pkg.name()))
                addPackage(pkg);
        }
    }
    emit configLoaded();
    return true;
}

bool PackageList::syncWithFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString fileName = _fileName.isEmpty() ? m_root + m_configFile : _fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;

    QTextStream in(&file);

    QString comment = in.readLine();
    QString format = in.readLine();
		
    Package *apkg;
    while (!in.atEnd())
    {
        Package pkg;
        if (pkg.read(in))
        {
            apkg = getPackage(pkg.name());
            if (apkg)
            {
                if (pkg.isInstalled(Package::BIN))
                    apkg->setInstalled(Package::BIN);
                if (pkg.isInstalled(Package::LIB))
                    apkg->setInstalled(Package::LIB);
                if (pkg.isInstalled(Package::DOC))
                    apkg->setInstalled(Package::DOC);
                if (pkg.isInstalled(Package::SRC))
                    apkg->setInstalled(Package::SRC);
            }
            else
                addPackage(pkg);
        }
    }
    emit configLoaded();
    return true;
}

bool PackageList::syncWithDatabase(Database &database)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QList<Package*>::iterator it;
    for (it = m_packageList.begin(); it != m_packageList.end(); ++it)
	{
		Package *apkg = (*it);
		Package *pkg = database.getPackage(apkg->name(), apkg->version().toAscii());
		if (!pkg)
			continue;
        if (pkg->isInstalled(Package::BIN))
            apkg->setInstalled(Package::BIN);
        if (pkg->isInstalled(Package::LIB))
            apkg->setInstalled(Package::LIB);
        if (pkg->isInstalled(Package::DOC))
            apkg->setInstalled(Package::DOC);
        if (pkg->isInstalled(Package::SRC))
            apkg->setInstalled(Package::SRC);
	}
    return true;
}

bool PackageList::readHTMLInternal(QIODevice *ioDev, PackageList::Type type, bool append)
{
    if (!append)
        m_packageList.clear();
    Downloader d(true); // for notes, maybe async?

    switch (type)
    {
    case PackageList::SourceForge:
        while (!ioDev->atEnd())
        {
            QByteArray line = ioDev->readLine();
            if (line.contains("<td><a href=\"/project/showfiles.php?group_id="))
            {
                Package pkg;
                int a = line.indexOf("\">") + 2;
                int b = line.indexOf("</a>");
                QByteArray name = line.mid(a,b-a);
                if(m_curSite && m_curSite->isExclude(name)) {
                    ioDev->readLine();
                    continue;
                }
                if (m_curSite)
					pkg.addDeps(m_curSite->getDependencies(name));
                pkg.setName(name);
                line = ioDev->readLine();
                a = line.indexOf("\">") + 2;
                b = line.indexOf("</a>");
                QByteArray version = line.mid(a,b-a);
                pkg.setVersion(version);
                // available types could not be determined on this web page
                // so assume all types are available
				Package::PackageItem item;
                item.set(m_baseURL,name+"-"+version+"-bin.zip",Package::BIN,false);
				pkg.add(item);
                item.set(m_baseURL,name+"-"+version+"-lib.zip",Package::LIB,false);
				pkg.add(item);
                item.set(m_baseURL,name+"-"+version+"-src.zip",Package::SRC,false);
				pkg.add(item);
                item.set(m_baseURL,name+"-"+version+"-doc.zip",Package::DOC,false);
				pkg.add(item);
                addPackage(pkg);
            }
        }
        break;

    case PackageList::ApacheModIndex:
        const char *lineKey1 = "alt=\"[   ]\"> <a href=\"";
        const char *lineKey2 = "alt=\"[   ]\" /> <a href=\"";
        const char *fileKeyStart = "<a href=\"";
        const char *fileKeyEnd = "\">";
        int i = 0;

        while (!ioDev->atEnd())
        {
            QByteArray line = ioDev->readLine();
            if (line.contains(lineKey1) || line.contains(lineKey2))
            {
                int a = line.indexOf(fileKeyStart) + strlen(fileKeyStart);
                int b = line.indexOf(fileKeyEnd,a);
                QByteArray path = line.mid(a,b-a);
                QByteArray ptype, name = path;

                // desktop-translations-10.1-41.3.noarch.rpm
                // kde3-i18n-vi-3.5.5-67.9.noarch.rpm
                // aspell-0.50.3-3.zip
                // bzip2-1.0.4.notes

                // first remove ending
                int idx = name.lastIndexOf('.');
                if(idx != -1)
                {
                    ptype = name.mid(idx + 1);
                    name = name.left(idx);
                }
                else
                {
                    ptype = "unknown";
                }

                // now split a little bit :)
                QList<QByteArray> parts;
                QList<QByteArray> tempparts = name.split('-');
                // and once more on '.'
                for (int i = 0; i < tempparts.size(); ++i)
                {
                    parts << tempparts[i].split('.');
                    parts << "-";
                }
                // not a correct named file
                if(parts.size() <= 1)
                    continue;

                parts.removeLast();

#ifdef _DEBUG

                qDebug() << parts.size();
                for (int i = 0; i < parts.size(); ++i)
                    qDebug() << parts.at(i);
#endif

                int iVersionLow = 0, iVersionHigh = parts.size() - 1;
                for(; iVersionHigh > 0; iVersionHigh --)
                {
                    if(parts[iVersionHigh][0] >= '0' && parts[iVersionHigh][0] <= '9')
                        break;
                }
                if(iVersionHigh == 0)
                    continue;
                iVersionHigh++;
                for(; iVersionLow < iVersionHigh; iVersionLow++)
                {
                    bool ok;
                    parts[iVersionLow].toInt(&ok);
                    if(ok)
                        break;
                }
                if(iVersionLow == iVersionHigh)
                    continue;
                QByteArray version, patchlevel, type;
                name = "";
                for(int i = 0; i < iVersionLow - 1; i++)
                {
                    if(name.size())
                        name += '.';
                    name += parts[i];
                }
                if(m_curSite && m_curSite->isExclude(name))
                    continue;
                for(; iVersionLow < iVersionHigh; iVersionLow++)
                {
                    if(parts[iVersionLow] == "-")
                        break;
                    if(version.size())
                        version += '.';
                    version += parts[iVersionLow];
                }
                iVersionLow++;
                for(; iVersionLow < iVersionHigh; iVersionLow++)
                {
                    if(parts[iVersionLow] == "-")
                        break;
                    if(patchlevel.size())
                        patchlevel += '.';
                    patchlevel += parts[iVersionLow];
                }
                // all after iVersionHigh is type
                if(iVersionHigh < parts.size())
                {
                    // should ever be true - have to think over :)
                    if(parts[iVersionHigh] == "-")
                        iVersionHigh++;
                    for(; iVersionHigh < parts.size(); iVersionHigh++)
                    {
                        if(parts[iVersionHigh] == "-")
                            break;
                        if(type.size())
                            type += '.';
                        type += parts[iVersionHigh];
                    }
                }
				// set type to bin in case no package type is given
				// this may result in ignoring the combined package 
				// if separated package are found before
				if (type.size() == 0) 
					type = "bin";

				if(!patchlevel.isEmpty())
                    version += "-" + patchlevel;

                Package *pkg = getPackage(name, version);
                if(ptype == "notes") {
                    // Download package
                    QByteArray ba;
                    d.start(m_baseURL + '/' + path, ba);
                    if(!pkg) {
                        Package p;
                        p.setVersion(version);
                        p.setName(name);
                        p.setNotes(QString::fromUtf8(ba));
                        addPackage(p);
                    } else {
                        pkg->setNotes(QString::fromUtf8(ba));
                    }
                } else {
                    if(!pkg) {
                        Package p;
                        p.setVersion(version);
                        p.setName(name);
						Package::PackageItem item;
                        item.set(m_baseURL, path, type);
						p.add(item);
                        addPackage(p);
                        if (m_curSite)
							p.addDeps(m_curSite->getDependencies(name));
                    } else {
						Package::PackageItem item;
						item.set(m_baseURL, path, type);
						pkg->add(item);
						if (m_curSite)
							pkg->addDeps(m_curSite->getDependencies(name));
                    }
                }
            }
        }
        break;
    }
    emit configLoaded();
    return true;
}

bool PackageList::readHTMLFromByteArray(const QByteArray &_ba, PackageList::Type type, bool append)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QByteArray ba(_ba);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;

    return readHTMLInternal(&buf, type, append);
}

bool PackageList::readHTMLFromFile(const QString &fileName, PackageList::Type type, bool append)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QFile pkglist(fileName);
    if (!pkglist.exists())
        return false;

    pkglist.open(QIODevice::ReadOnly);

    return readHTMLInternal(&pkglist, type, append);
}

bool PackageList::setInstalledPackage(const Package &apkg)
{
    Package *pkg = getPackage(apkg.name());
    if (!pkg)
    {
        qDebug() << __FUNCTION__ << "package " << apkg.name() << " not found";
        return false;
    }
    pkg->setInstalled(apkg);
    return true;
}

void PackageList::dump(const QString &title)
{
    qDebug() << "class PackageList dump: " << title;
    qDebug() << "m_name       " << m_name;
    qDebug() << "root         " << m_root;
    qDebug() << "m_configFile " << m_configFile;
    qDebug() << "m_baseURL    " << m_baseURL;
    QList<Package*>::ConstIterator it = m_packageList.constBegin();
    for ( ; it != m_packageList.constEnd(); ++it)
        (*it)->dump();
}

#include "packagelist.moc"
