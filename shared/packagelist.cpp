/****************************************************************************
**
** Copyright (C) 2005-2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
#include "downloader.h"
#include "installer.h"

PackageList::PackageList(Downloader *_downloader)
        : QObject()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    downloader = _downloader ? _downloader : new Downloader;
    root = ".";
    m_configFile = "/packages.txt";

}

PackageList::~PackageList()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
}

bool PackageList::hasConfig()
{
    return QFile::exists(root + m_configFile);
}

void PackageList::addPackage(const Package &package)
{
#ifdef DEBUG
    Package pkg = package;
    qDebug() << __FUNCTION__ << pkg.toString();
#endif

    m_packageList.append(package);
}

Package *PackageList::getPackage(const QString &pkgName, const QByteArray &version)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QList<Package>::iterator it = m_packageList.begin();
    for ( ; it != m_packageList.end(); ++it)
        if (it->name() == pkgName) {
            if(!version.isEmpty() && it->version() != version)
                continue;
            return &(*it);
        }
    return NULL;
}

void PackageList::listPackages(const QString &title)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    qDebug() << title;
    QList<Package>::iterator i;
    for (i = m_packageList.begin(); i != m_packageList.end(); ++i)
        qDebug(i->toString(true," - ").toLatin1());
}

bool PackageList::writeToFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    if (m_packageList.count() == 0)
        return false;

    QString fileName = _fileName.isEmpty() ? root + m_configFile : _fileName;
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
    QList<Package>::iterator i;
    for (i = m_packageList.begin(); i != m_packageList.end(); ++i)
        i->write(out);
    return true;
}

bool PackageList::readFromFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString fileName = _fileName.isEmpty() ? root + m_configFile : _fileName;
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
            addPackage(pkg);
    }
    emit loadedConfig();
    return true;
}

bool PackageList::syncWithFile(const QString &_fileName)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString fileName = _fileName.isEmpty() ? root + m_configFile : _fileName;
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
    emit loadedConfig();
    return true;
}


bool PackageList::readHTMLInternal(QIODevice *ioDev, PackageList::Type type)
{
    m_packageList.clear();


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
                if(m_curSite->isExclude(name))
                    continue;
                pkg.addDeps(m_curSite->getDependencies(name));
                pkg.setName(name);
                line = ioDev->readLine();
                a = line.indexOf("\">") + 2;
                b = line.indexOf("</a>");
                QByteArray version = line.mid(a,b-a);
                pkg.setVersion(version);
                // available types could not be determined on this web page
                // so assume all types are available
                pkg.add(m_baseURL+name+"-"+version+"-bin.zip",Package::BIN,false);
                pkg.add(m_baseURL+name+"-"+version+"-lib.zip",Package::LIB,false);
                pkg.add(m_baseURL+name+"-"+version+"-src.zip",Package::SRC,false);
                pkg.add(m_baseURL+name+"-"+version+"-doc.zip",Package::DOC,false);
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
                if(m_curSite->isExclude(name))
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
                if(!patchlevel.isEmpty())
                    version += "-" + patchlevel;

                Package *pkg = getPackage(name, version);
                if(!pkg) {
                    Package p;
                    p.setVersion(version);
                    p.setName(name);
                    p.add(m_baseURL + path, type);
                    addPackage(p);
                    p.addDeps(m_curSite->getDependencies(name));
                } else {
                    pkg->add(m_baseURL + path, type);
                    pkg->addDeps(m_curSite->getDependencies(name));
                }
            }
        }
        break;
    }
    emit loadedConfig();
    return true;
}

bool PackageList::readHTMLFromByteArray(const QByteArray &_ba, PackageList::Type type)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QByteArray ba(_ba);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;

    return readHTMLInternal(&buf, type);
}

bool PackageList::readHTMLFromFile(const QString &fileName, PackageList::Type type )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QFile pkglist(fileName);
    if (!pkglist.exists())
        return false;

    pkglist.open(QIODevice::ReadOnly);

    return readHTMLInternal(&pkglist, type);
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

bool PackageList::downloadPackage(const QString &pkgName, Package::Types types)
{
    qDebug() << __FUNCTION__ << " " << pkgName; 
    Package *pkg = getPackage(pkgName);
    if (!pkg) 
    {
    	  qDebug() << __FUNCTION__ << " package not found";
        return false;
    }
    // FIXME: handle error code
#ifdef DEBUG
    qDebug() << types;
#endif
    if (types & Package::BIN)
        pkg->downloadItem(downloader,Package::BIN);
    if (types & Package::LIB)
        pkg->downloadItem(downloader,Package::LIB);
    if (types & Package::DOC)
        pkg->downloadItem(downloader,Package::DOC);
    if (types & Package::SRC)
        pkg->downloadItem(downloader,Package::SRC);
    return true;
}

bool PackageList::installPackage(const QString &pkgName, Package::Types types)
{
    Package *pkg = getPackage(pkgName);
    if (!pkg)
        return false;
    // FIXME: handle error code
    if (types & Package::BIN)
        pkg->installItem(installer,Package::BIN);
    if (types & Package::LIB)
        pkg->installItem(installer,Package::LIB);
    if (types & Package::DOC)
        pkg->installItem(installer,Package::DOC);
    if (types & Package::SRC)
        pkg->installItem(installer,Package::SRC);
    return true;
}

void PackageList::dump(const QString &title)
{
    qDebug() << "class PackageList dump: " << title;
    qDebug() << "m_name       " << m_name;
    qDebug() << "root         " << root;
    qDebug() << "m_configFile " << m_configFile;
    qDebug() << "m_baseURL    " << m_baseURL;
    QList<Package>::iterator it = m_packageList.begin();
    for ( ; it != m_packageList.end(); ++it)
        it->dump();
}

#include "packagelist.moc"
