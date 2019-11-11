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

#include "database.h"
#include "debug.h"
#include "downloader.h"
#include "globalconfig.h"
#include "installerengine.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "misc.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QtXml>

#ifdef USE_GUI
#include <QStandardItemModel>
#include <QTreeWidget>
#endif

typedef QMap<QString,QString> PackagePathMap;

class XmlHandlerBase : public QXmlDefaultHandler
{
public:
    XmlHandlerBase()
        : m_level(0)
    {
    }

    virtual ~XmlHandlerBase()
    {
    }

    bool parse(QIODevice *device)
    {
        QXmlSimpleReader xmlReader;
        xmlReader.setContentHandler(this);
        xmlReader.setErrorHandler(this);
        QXmlInputSource source(device);
        bool ok = xmlReader.parse(&source);
        return ok;
    }

    const QUrl &url() { return m_url; }

    protected:
        QString element;
        bool m_inElement;
        QString m_parent;
        QString m_last;
        int m_level;
        QUrl m_url;

        bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
        {
            Q_UNUSED(namespaceURI);
            Q_UNUSED(localName);

            m_inElement = true;
            element = qName;
            m_last = qName;
            return true;
        }

        bool characters ( const QString & ch )
        {
            if  (!m_inElement)
                return true;

            return true;
        }

        bool error( const QXmlParseException & exception )
        {
            qDebug() << exception.lineNumber() << exception.columnNumber() << exception.message();
            return  true;
        }

        bool fatalError ( const QXmlParseException & exception )
        {
            qCritical() << exception.lineNumber() << exception.columnNumber() << exception.message();
            return  false;
        }

        bool warning ( const QXmlParseException & exception )
        {
            qWarning() << exception.lineNumber() << exception.columnNumber() << exception.message();
            return  true;
        }
};


class JenkinsJobsXmlHandler : public XmlHandlerBase
{
public:
    JenkinsJobsXmlHandler(PackagePathMap &packages, const QString &filter)
        : m_packages(packages)
        , m_filter(filter.toLower())
    {
    }

    virtual ~JenkinsJobsXmlHandler()
    {
    }

    bool characters(const QString & ch)
    {
        if  (!m_inElement)
            return true;
        // handle in element data
        if (element == "name") {
            QString s = ch.toLower();
            if (s.contains(m_filter)) {
                s.replace("_" + m_filter, "");
                m_packages[s] = ch;
            }
        }

        return true;
    }

    protected:
        PackagePathMap &m_packages;
        QString m_filter;
};

class JenkinsJobXmlHandler : public XmlHandlerBase
{
public:
    JenkinsJobXmlHandler(const QString &jobName)
        : m_jobName(jobName)
    {
    }

    virtual ~JenkinsJobXmlHandler()
    {
    }

    bool load(Site *site)
    {
        QByteArray ba;
        QUrl url(site->url().toString() + QString("/job/%1/api/xml").arg(m_jobName));
        if (!Downloader::instance()->fetch(url, ba)) {
            return false;
        }
        QXmlSimpleReader xmlReader;
        xmlReader.setContentHandler(this);
        xmlReader.setErrorHandler(this);
        QXmlInputSource source;
        source.setData(ba);
        bool ok = xmlReader.parse(&source);
        qDebug() << m_url;
        return ok;
    }

    protected:
        QString m_jobName;

        bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts)
        {
            Q_UNUSED(namespaceURI);
            Q_UNUSED(localName);

            element = qName;
            m_last = qName;
            qDebug() << __FUNCTION__ << qName << m_level;
            if (element == "lastSuccessfulBuild")
                m_inElement = true;
            m_level++;
            return true;
        }

        bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
        {
            qDebug() << __FUNCTION__ << qName << m_level;
            if (qName == "lastSuccessfulBuild")
                m_inElement = false;
            m_level--;
            return true;
        }

        bool characters(const QString & ch)
        {
            qDebug() << __FUNCTION__ << m_level << ch;
            // handle in element data
            if (m_inElement && !m_url.isValid() && element == "url")
                m_url = QUrl::fromUserInput(ch);
            return true;
        }
};

class JenkinsBuildArtifactHandler : public XmlHandlerBase
{
public:
    JenkinsBuildArtifactHandler()
    {
    }

    virtual ~JenkinsBuildArtifactHandler()
    {
    }

    bool load(const QUrl &url)
    {
        QByteArray ba;
        m_url = url;
        if (!Downloader::instance()->fetch(url.toString() + "/api/xml", ba)) {
            return false;
        }
        QXmlSimpleReader xmlReader;
        xmlReader.setContentHandler(this);
        xmlReader.setErrorHandler(this);
        QXmlInputSource source;
        source.setData(ba);
        bool ok = xmlReader.parse(&source);
        qDebug() << m_url;
        return ok;
    }

    const QStringList &fileNames() { return m_fileNames; }

    protected:
        QStringList m_fileNames;

        bool startElement(const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts)
        {
            Q_UNUSED(namespaceURI);
            Q_UNUSED(localName);

            element = qName;
            m_last = qName;
            if (element == "artifact")
                m_inElement = true;
            m_level++;
            return true;
        }

        bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
        {
            if (qName == "artifact")
                m_inElement = false;
            m_level--;
            return true;
        }

        bool characters(const QString & ch)
        {
            if (m_inElement && element == "fileName") {
                QString value = ch;
                value.replace("64-cl","64");
                value.replace("_64.7z","_64-bin.7z");
                value.replace("-windows", "");
                m_fileNames << value;
            }
            return true;
        }
};

PackageList::PackageList(InstallerEngine *parent)
        : QObject(),  m_parent(parent)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    m_root = ".";
    m_configFile = "/packages.txt";
    m_curSite = 0;
    m_parserConfigFileFound = 0;
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

bool PackageList::append(const Package &package)
{
    m_packageList.append(new Package(package));
    return true;
}

bool PackageList::append(Package &package)
{
    m_packageList.append(new Package(package));
    return true;
}

bool PackageList::append(const PackageList &src)
{
    Q_FOREACH ( const Package *p, src.m_packageList ) 
    {
        append(*p);
    }
    return true;
}

Package *PackageList::find(const QString &name, const QByteArray &version)
{
    return find(PackageInfo::fromString(name, version));
}

Package *PackageList::find(const PackageInfo &info)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    QString name = info.name;
    if (!info.compiler.isEmpty())
        name += '-' + info.compiler;
    Q_FOREACH( Package *p, m_packageList )
    {
        if (p->name() == name) {
            if(!info.version.isEmpty() && p->version() != info.version)
                continue;
            return p;
        }
    }
    return NULL;
}


QStringList PackageList::listPackages()
{
    QStringList list;
    Q_FOREACH ( const Package *p, m_packageList )
        list << p->toString(true," - ");
    return list;
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
    // also maybe move this into class Package -> FileTypes::write(QTextStream &out)
    QTextStream out(&file);
    out << "# package list" << "\n";
    out << "@format 1.0" << "\n";
    Q_FOREACH ( const Package *p, m_packageList )
        p->write(out);
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
            if ( pkg.isInstalled(FileTypes::BIN) || (m_curSite && !m_curSite->isExclude(pkg.name())))
                append(pkg);
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
            apkg = find(pkg.name());
            if (apkg)
            {
                if (pkg.isInstalled(FileTypes::BIN))
                    apkg->setInstalled(FileTypes::BIN);
                if (pkg.isInstalled(FileTypes::LIB))
                    apkg->setInstalled(FileTypes::LIB);
                if (pkg.isInstalled(FileTypes::DOC))
                    apkg->setInstalled(FileTypes::DOC);
                if (pkg.isInstalled(FileTypes::SRC))
                    apkg->setInstalled(FileTypes::SRC);
                if (pkg.isInstalled(FileTypes::DBG))
                    apkg->setInstalled(FileTypes::DBG);
            }
            else
                append(pkg);
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

    QList<Package*> newPackages;
    Q_FOREACH ( Package *apkg, m_packageList )
    {
        Package *pkg = database.getPackage(apkg->name(), apkg->version().toString().toLatin1());
        if (!pkg)
        {
            pkg = database.getPackage(apkg->name());
            if (!pkg || pkg->handled())
                continue;
            // if this installed package is already in the package list
            // may be it comes later in the list, don't add it twice
            Package *p = find(pkg->name(),pkg->version().toString().toLatin1());
            if (p)
                continue;
            newPackages += pkg;
            pkg->setHandled(true);
            continue;
        }
        if (pkg->isInstalled(FileTypes::BIN))
            apkg->setInstalled(FileTypes::BIN);
        if (pkg->isInstalled(FileTypes::LIB))
            apkg->setInstalled(FileTypes::LIB);
        if (pkg->isInstalled(FileTypes::DOC))
            apkg->setInstalled(FileTypes::DOC);
        if (pkg->isInstalled(FileTypes::SRC))
            apkg->setInstalled(FileTypes::SRC);
        if (pkg->isInstalled(FileTypes::DBG))
            apkg->setInstalled(FileTypes::DBG);
        pkg->setHandled(true);
    }
    Q_FOREACH ( Package *p, newPackages )
        m_packageList += p;

    return true;
}

inline bool isPackageFileName(const QString &fileName)
{
    return ( fileName.endsWith(".zip") || fileName.endsWith(".tbz") || fileName.endsWith(".tar.bz2") || fileName.endsWith(".7z"));
}

inline bool isPackageFileName(const QByteArray &fileName)
{
    return ( fileName.endsWith(".zip") || fileName.endsWith(".tbz") || fileName.endsWith(".tar.bz2") || fileName.endsWith(".7z") );
}

class FileType {
    public:
        FileType(const QString &v, const QString &f) { version = v; fileName = f; }
    QString version;
    QString fileName;
};

// filter List to get latest versions
QStringList filterFileName(const QStringList &files, CompilerTypes::Type currentCompiler)
{
    QStringList filteredFiles;
    QString currentCompilerString = allCompilers.toString(currentCompiler);

    // key is <package>-<compiler>-<type> eg akonadi-vc100-bin
    QMap<QString,FileType*> packages;

    Q_FOREACH(const QString &fileName, files)
    {
        if (isPackageFileName(fileName))
        {
            QString pkgName;
            QString pkgCompiler;
            QString pkgVersion;
            QString pkgType;
            QString pkgFormat;
            if (!PackageInfo::fromFileName(fileName, pkgName, pkgCompiler, pkgVersion, pkgType, pkgFormat))
                continue;
            // skip packages for other compilers
            if (!pkgCompiler.isEmpty() && pkgCompiler != currentCompilerString)
                continue;

            QString key = pkgName;
            if (!pkgCompiler.isEmpty())
                key += "-" + pkgCompiler;
            key += "-" + pkgType;

            if (packages.contains(key))
            {
                qDebug() << "compare" << fileName << "with" << key << packages.value(key)->version << pkgVersion << (packages.value(key)->version < pkgVersion);
                if (packages.value(key)->version < pkgVersion)
                {
                    qDebug() << "using" << fileName << "as higher version";
                    packages[key]->version = pkgVersion;
                    packages[key]->fileName = fileName;
                }
            }
            else
            {
                qDebug() << "added" << fileName << "as" << key;
                packages[key] = new FileType(pkgVersion,fileName);
            }
        }
    }
    Q_FOREACH(const FileType *p,packages)
        filteredFiles << p->fileName;
    qDeleteAll(packages);

    return filteredFiles;
}

bool PackageList::readFromDirectory(const QString &dir, bool append)
{
    Q_ASSERT(m_parent);

    if (!append)
        m_packageList.clear();

    QStringList files;
    QDir d(dir);

    if (!d.exists())
        return false;

    d.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    d.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = d.entryInfoList();
    for (int i = 0; i < list.size(); ++i) 
    {
        QFileInfo fi = list.at(i);
        files << fi.fileName();
    }
    files = filterFileName(files, m_parent->currentCompiler());
    addPackagesFromFileNames(files);

    emit configLoaded();
    m_parserConfigFileFound = false;
    return true;
}

bool PackageList::readInternal(QIODevice *ioDev, PackageList::Type type, bool append)
{
    Q_ASSERT(m_parent);

    if (!append)
        m_packageList.clear();

    m_parserConfigFileFound = false;
    QStringList files;

    switch (type)
    {
    
    case PackageList::Ftp:
        // -rw-r--r--    1 10004    10004      385455 Feb 04 23:00 amarok-mingw-1.80.20080121-lib.tar.bz2
        while (!ioDev->atEnd())
        {
            const QString line = QString::fromUtf8(ioDev->readLine().replace("\r\n","").replace("\n",""));
            const QStringList parts = line.split(' ',QString::SkipEmptyParts);
            int size = parts.size();
            if (size != 9)
                continue;
            // size could be used for download estimation 
            //QString size = parts[4];
            const QString &file = parts[8];
            if (!isPackageFileName(file))
                continue;
            files << file;
        }
        files = filterFileName(files, m_parent->currentCompiler());
        addPackagesFromFileNames(files);
        break;

    // <hudson _class='hudson.model.Hudson'><job _class='org.jenkinsci.plugins.workflow.job.WorkflowJob'><name>Adwaita_arm_flatpak</name></job><
    case PackageList::JenkinsXml: {
        PackagePathMap packages;
        JenkinsJobsXmlHandler jobs(packages, "release_win64");
        bool ok = jobs.parse(ioDev);
        qDebug() << packages;
        foreach(const QString &package, packages.keys()) {
            JenkinsJobXmlHandler handler(packages[package]);
            ok = handler.load(m_curSite);
            if (!ok)
                continue;
            JenkinsBuildArtifactHandler buildHandler;
            if (!buildHandler.load(handler.url()))
                continue;
            qDebug() << buildHandler.fileNames();
            files = filterFileName(buildHandler.fileNames(), CompilerTypes::MSVC141_X64);
            //addPackagesFromFileNames(files);
        }
        break;
    }

    case PackageList::SourceForge:
        while (!ioDev->atEnd())
        {
            QByteArray line = ioDev->readLine();
            if (line.contains("<td><a href=\"/project/showfiles.php?group_id="))
            {
                Package pkg;
                int a = line.indexOf("\">") + 2;
                int b = line.indexOf("</a>");
                const QByteArray name = line.mid(a,b-a);
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
                const QByteArray version = line.mid(a,b-a);
                pkg.setVersion(version);
                // available types could not be determined on this web page
                // so assume all types are available
                {
                    Package::PackageItem item(FileTypes::BIN);
                    item.setUrl(m_baseURL);
                    item.setFileName(name+"-"+version+"-bin.zip");
                    pkg.add(item);
                }
                {
                    Package::PackageItem item(FileTypes::LIB);
                    item.setUrl(m_baseURL);
                    item.setFileName(name+"-"+version+"-lib.zip");
                    pkg.add(item);
                }
                {
                    Package::PackageItem item(FileTypes::DOC);
                    item.setUrl(m_baseURL);
                    item.setFileName(name+"-"+version+"-doc.zip");
                    pkg.add(item);
                }
                {
                    Package::PackageItem item(FileTypes::SRC);
                    item.setUrl(m_baseURL);
                    item.setFileName(name+"-"+version+"-src.zip");
                    pkg.add(item);
                }
                {
                    Package::PackageItem item(FileTypes::DBG);
                    item.setUrl(m_baseURL);
                    item.setFileName(name+"-"+version+"-dbg.zip");
                    pkg.add(item);
                }
                addPackage(pkg);
            }
        }
        break;

    // for example http://www.mirrorservice.org/sites/download.sourceforge.net/pub/sourceforge/k/kd/kde-cygwin/
    case PackageList::SourceForgeMirror:
    case PackageList::ApacheModIndex:
    case PackageList::Default:
        const char *startKey1 = "<a href=\"";
        const char *startKey2 = "<A HREF=\"";
        const char *endKey = "\">";
        const QByteArray data = ioDev->readAll();
        int startKeyLength = strlen(startKey1);
        int endKeyLength = strlen(endKey);
        QUrl baseURL(m_baseURL);
        int b = 0 , i;
        const int ds = data.size();
        for (int a = 0 ; a < ds; a = b + endKeyLength)
        {
            if ((i = data.indexOf(startKey1,a)) == -1 && (i = data.indexOf(startKey2,a)) == -1)
                break;
            a = i + startKeyLength;
            b = data.indexOf(endKey,a);
            QUrl url = QUrl(data.mid(a,b-a));
            if (!url.isValid())
                continue;
            const QString path = url.path();
            if (path.isEmpty())
                continue;
            if (!isPackageFileName(path))
                continue;
            int c = path.lastIndexOf('/');
            if (c != -1)
                 files << path.mid(c+1);
            else
                 files << path;
        }
        files = filterFileName(files, m_parent->currentCompiler());
        addPackagesFromFileNames(files);
        break;
    }
    emit configLoaded();
    m_parserConfigFileFound = false;
    return true;
}

bool PackageList::addPackagesFromFileNames(const QStringList &files, bool ignoreConfigTxt)
{
    if (!ignoreConfigTxt && files.contains("config.txt"))
    {
        // fetch config file
        QFileInfo cfi(Settings::instance().downloadDir()+"/config-temp.txt");
        bool ret = Downloader::instance()->fetch(m_baseURL.toString() + "/config.txt",cfi.absoluteFilePath());
        if (ret)
        {
            GlobalConfig g;
            g.setBaseURL(m_baseURL);
            const QStringList configFile = QStringList() << cfi.absoluteFilePath();
            if (!g.parse(configFile))
                return false;

            // add package from globalconfig
            Q_FOREACH( const Package *pkg, *g.packages() )
            {
                append(*pkg);
            }
            // sites are not supported here
            m_parserConfigFileFound = true;
            // config.txt overrides all other definitions
            return true;
        }
    }

    Q_FOREACH(const QString &fileName, files)
    {
#ifdef ENABLE_HINTFILE_SUPPORT
        // @TODO using hint files results into duplicated package entries
        if (fileName.endsWith(".hint"))
        {
            addPackageFromHintFile(fileName);
        } else
#endif
        if (isPackageFileName(fileName)) {
            QString pkgName;
            QString pkgCompiler;
            QString pkgVersion;
            QString pkgType;
            QString pkgFormat;
            if (!PackageInfo::fromFileName(fileName, pkgName, pkgCompiler, pkgVersion, pkgType, pkgFormat))
                continue;

            QString key = pkgName;
            if (!pkgCompiler.isEmpty())
                key += "-" + pkgCompiler;

            Package *pkg = find(key, pkgVersion.toLatin1());
            if(!pkg) {
                Package p;
                p.setVersion(pkgVersion);
                p.setName(key);
                Package::PackageItem item(pkgType);
                item.setUrlAndFileName(m_baseURL.toString() + fileName,fileName);
                p.add(item);
                if(m_curSite)
                    p.setNotes(m_curSite->packageNote(key));
                if(m_curSite)
                    p.setLongNotes(m_curSite->packageLongNotes(key));
                if(m_curSite)
                    p.setHomeURL(m_curSite->packageHomeUrl(key));
                if (m_curSite)
                {
#ifdef VERSIONED_DEPENDENCIES
                    p.addDeps(m_curSite->getDependencies(key+"-"+pkgVersion));
#endif
                    p.addDeps(m_curSite->getDependencies(key));
                }
                append(p);
            } else {
                Package::PackageItem item(pkgType);
                item.setUrlAndFileName(m_baseURL.toString() + fileName,fileName);
                pkg->add(item);
                if(m_curSite)
                    pkg->setNotes(m_curSite->packageNote(key));
                if(m_curSite)
                    pkg->setLongNotes(m_curSite->packageLongNotes(key));
                if(m_curSite)
                    pkg->setHomeURL(m_curSite->packageHomeUrl(key));
                if (m_curSite)
                {
#ifdef VERSIONED_DEPENDENCIES
                    pkg->addDeps(m_curSite->getDependencies(key+"-"+pkgVersion));
#endif
                    pkg->addDeps(m_curSite->getDependencies(key));
                }
            }
        }
        else {
            qDebug() << __FUNCTION__ << "unsupported package format" << fileName;
        }
    }
    return true;
}

bool PackageList::readFromByteArray(const QByteArray &_ba, PackageList::Type type, bool append)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QByteArray ba(_ba);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;

    return readInternal(&buf, type, append);
}

bool PackageList::readFromFile(const QString &fileName, PackageList::Type type, bool append)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QFile pkglist(fileName);
    if (!pkglist.exists())
        return false;

    pkglist.open(QIODevice::ReadOnly);

    return readInternal(&pkglist, type, append);
}

bool PackageList::setInstalledPackage(const Package &apkg)
{
    Package *pkg = find(apkg.name());
    if (!pkg)
    {
        qDebug() << __FUNCTION__ << "package " << apkg.name() << " not found";
        return false;
    }
    pkg->setInstalled(apkg);
    return true;
}

void PackageList::clear()
{
    qDeleteAll(m_packageList);
    m_packageList.clear();
    m_curSite = 0;
}

QDebug & operator<<(QDebug &out, PackageList &c)
{
    out << "PackageList( "
        << "m_name:" << c.m_name
        << "m_root:" << c.m_root
        << "m_configFile:" << c.m_configFile
        << "m_baseURL:" << c.m_baseURL
        << "packages: (size:" << c.m_packageList.size();
    Q_FOREACH( const Package *pkg, c.m_packageList )
        out << *pkg;
    out << ") )";
    return out;
}
