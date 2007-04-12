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
    qDeleteAll(m_packageList);
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

bool PackageList::append(PackageList &src)
{
    QList<Package*>::iterator it;
	for (it = src.m_packageList.begin(); it != src.m_packageList.end(); ++it)
	{
		Package *pkg = (*it);
		addPackage(*pkg);
	}
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
		{
			pkg = database.getPackage(apkg->name());
			if (!pkg)
				continue;
			m_packageList.append(pkg);
			pkg->setHandled(true);
			continue;
		}
        if (pkg->isInstalled(Package::BIN))
            apkg->setInstalled(Package::BIN);
        if (pkg->isInstalled(Package::LIB))
            apkg->setInstalled(Package::LIB);
        if (pkg->isInstalled(Package::DOC))
            apkg->setInstalled(Package::DOC);
        if (pkg->isInstalled(Package::SRC))
            apkg->setInstalled(Package::SRC);
		pkg->setHandled(true);
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
                int a = line.lastIndexOf(fileKeyStart) + strlen(fileKeyStart);
                int b = line.indexOf(fileKeyEnd,a);
                QByteArray fileName = line.mid(a,b-a);

                // desktop-translations-10.1-41.3.noarch.rpm
                // kde3-i18n-vi-3.5.5-67.9.noarch.rpm
                // aspell-0.50.3-3.zip
                // bzip2-1.0.4.notes

				QString pkgName;
				QString pkgVersion;
				QString pkgType;
				QString pkgFormat;
				if (!PackageInfo::fromFileName(fileName,pkgName,pkgVersion,pkgType,pkgFormat))
					continue;
				Package *pkg = getPackage(pkgName, pkgVersion.toAscii());
                if(pkgFormat == "notes") {
                    // Download package
                    QByteArray ba;
                    d.start(m_baseURL + '/' + fileName, ba);
                    if(!pkg) {
                        Package p;
                        p.setVersion(pkgVersion);
                        p.setName(pkgName);
                        p.setNotes(QString::fromUtf8(ba));
                        addPackage(p);
                    } else {
                        pkg->setNotes(QString::fromUtf8(ba));
                    }
                } else {
                    if(!pkg) {
                        Package p;
                        p.setVersion(pkgVersion);
                        p.setName(pkgName);
						Package::PackageItem item;
                        item.set(m_baseURL + '/' + fileName, "", pkgType.toAscii());
						p.add(item);
                        addPackage(p);
                        if (m_curSite)
							p.addDeps(m_curSite->getDependencies(pkgName));
                    } else {
						Package::PackageItem item;
						item.set(m_baseURL + '/' + fileName, "", pkgType.toAscii());
						pkg->add(item);
						if (m_curSite)
							pkg->addDeps(m_curSite->getDependencies(pkgName));
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
