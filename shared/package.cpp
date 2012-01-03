/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker <ralf.habacker@freenet.de>
** Copyright (C) 2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
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

#include "config.h"
#include "debug.h"
#include "package.h"
#include "packageinfo.h"
#include "downloader.h"
#include "hashfile.h"
#include "installer.h"
#include "database.h"
#include "misc.h"
#include "uninstaller.h"

#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMap>

Package::PackageVersion::PackageVersion(const QString &version)
{
    m_version = version;
}

Package::PackageVersion &Package::PackageVersion::operator=(const Package::PackageVersion &other)
{
    m_version = other.m_version;
    return *this; 
}

int  Package::PackageVersion::compare(const Package::PackageVersion &other) const
{
    int result;
    /// @TODO add version specifc compare here when required e.g. 4.00.80 is equal to 4.0.80
    if (m_version < other.m_version)
        result = -1;
    else if (m_version > other.m_version)
        result = 1;
    else 
        result = 0;
    return result;
}

bool Package::PackageVersion::operator<(const Package::PackageVersion &other) const
{
    return compare(other) < 0;
}

bool Package::PackageVersion::operator<=(const Package::PackageVersion &other) const
{
    return compare(other) <= 0;
}

bool Package::PackageVersion::operator>(const Package::PackageVersion &other) const
{
    return compare(other) > 0; 
}

bool Package::PackageVersion::operator>=(const Package::PackageVersion &other) const
{
    return compare(other) >= 0; 
}

bool Package::PackageVersion::operator==(const Package::PackageVersion &other) const
{
    return compare(other) == 0;
}

bool Package::PackageVersion::operator!=(const Package::PackageVersion &other) const
{
    return compare(other) != 0;
}

bool Package::PackageVersion::operator==(const QString &other) const
{
    return compare(PackageVersion(other)) == 0; 
}

bool Package::PackageVersion::operator!=(const QString &other) const
{
    PackageVersion a(other);
    return compare(a) != 0;
}

bool Package::PackageVersion::isEmpty() const
{
    return m_version.isEmpty();
}

QString Package::PackageVersion::toString()  const
{
    return m_version;
}

QDebug &operator<<(QDebug &out, const Package::PackageVersion &c)
{
    out << "PackageVersion ("
        << "m_version" << c.m_version
        << ")";
    return out;
}

#ifndef PACKAGE_SMALL_VERSION
/**
  initiate package item
  @param url url for downloading
  @param fn local package item filename
  @param contentType package item content type as string
  @param bInstalled flag if package item is installed
 */
bool Package::PackageItem::set(const QUrl &url, const QString &fn, const QByteArray &contentType, bool bInstalled)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << " " << path << " " << contentType << " " << bInstalled;
#endif
    FileTypes::Type  type = FileTypes::fromString(contentType.toLower());
    return type == FileTypes::NONE?false:set(url, fn, type, bInstalled);

}

/**
  initiate package item
  @param url url for downloading
  @param fn local package item filename
  @param contentType package item content type
  @param bInstalled flag if package item is installed
 */
bool Package::PackageItem::set(const QUrl &url, const QString &fn, FileTypes::Type contentType, bool bInstalled)
{
    PackageItem desc;
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << path << fn << contentType << bInstalled;
#endif
    // generate fileName from from url when not set previously 
    if (m_fileName.isEmpty()) 
    {
        if(fn.isEmpty())
        {
            QString sUrl = url.toString();
            idx = sUrl.lastIndexOf('/');
            if(idx == -1) {
                qDebug() << __FUNCTION__ << "Invalid - no complete url found " << url << "packageitem ignored";    // FIXME
                return false;
            }
            desc.m_fileName = sUrl.mid(idx + 1);
        }
        else
            desc.m_fileName = fn;
    }
    
    idx = desc.m_fileName.lastIndexOf('.');
    if(idx == -1) {
        qDebug() << __FUNCTION__ << "Invalid - dot in filename expected" << desc.m_fileName;    // FIXME
        return false;
    }

    desc.m_url = url;
    if (contentType != FileTypes::NONE)
        desc.m_contentType = contentType;
    desc.m_installed  = m_installed;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << desc.m_contentType << desc.m_installed;
#endif
    *this = desc;
    return true;
}

bool Package::PackageItem::setUrlAndFileName(const QUrl &_url, const QString &fn)
{
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << path << fn;
#endif
    // if filename is set store it unconditional 
    if(!fn.isEmpty()) 
    {
        m_url = _url;
        m_fileName = fn; 
        return true;
    }

    // generate fileName from from url when not set previously 
    if (m_fileName.isEmpty()) 
    {
        QString path = _url.path();
        idx = path.lastIndexOf('/');
        if(idx == -1) {
            qCritical() << __FUNCTION__ << "could not set filename from url" << _url;
            return false;
        }
        m_fileName = path.mid(idx + 1);
        m_url = _url;
    }    
    return true;
}

bool Package::PackageItem::setContentType(const QString &type)
{
    m_contentType = FileTypes::fromString(type.toLower());
	return m_contentType != FileTypes::NONE;
}
#endif

QDebug &operator<<(QDebug &out, const FileTypes::Type c)
{
	out <<  FileTypes::toString(c);
    return out;
}

QDebug &operator<<(QDebug &out, const Package::PackageItem &c)
{
    out << "PackageItem ("
        << "m_url:" << c.m_url
        << "m_fileName:" << c.m_fileName
        << "m_contentType:" << FileTypes::toString(c.m_contentType)
        << "m_installed:" << c.m_installed
        << ")";
    return out;
}

#ifndef PACKAGE_SMALL_VERSION

Package::Package() : m_hashType(Hash::None)
{
    m_handled = false;
    m_userData[0] = 0;
    m_userData[1] = 0;
}

Package::Package(const Package &other)
{
    m_packages   = other.m_packages;
    m_name       = other.m_name;
    m_version    = other.m_version;
    m_categories = other.m_categories;
    m_deps       = other.m_deps;
    m_pathRelocs = other.m_pathRelocs;
    m_handled    = other.m_handled;
    m_notes      = other.m_notes;
    m_longNotes  = other.m_longNotes;
    m_homeUrl    = other.m_homeUrl;
    m_installedversion = other.m_installedversion;
    m_hashType = other.m_hashType;
    m_userData[0] = other.m_userData[0];
    m_userData[1] = other.m_userData[1];
}

QString Package::localFileName(FileTypes::Type contentType) const
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].fileName();
    return QString();
}

QUrl Package::getUrl(FileTypes::Type contentType) const
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].url();
    return QUrl();
}

bool Package::setUrl(FileTypes::Type contentType, const QUrl &url)
{
    if(!m_packages.contains(contentType))
        return false;

    m_packages[contentType].setUrl(url);
    return true;
}

bool Package::add(const PackageItem &item)
{
    if(m_packages.contains(item.contentType())) {
        qDebug() << __FUNCTION__ << m_name << " type " << item.contentType() << "already added";
        return false;
    }
    m_packages[item.contentType()] = item;
    return true;
}

Package::PackageItem &Package::item(FileTypes::Type contentType)
{
    static PackageItem empty;
    if(!m_packages.contains(contentType))
        return empty;

    return m_packages[contentType];
}

void Package::setInstalled(const Package &other)
{
    // FIXME!
}

bool Package::hasType(FileTypes::Type contentType) const
{
    return m_packages.contains(contentType);
}

QString Package::toString(bool installed, const QString &delim) const
{
    QString result = m_name + delim + m_version.toString();
    QString availableTypes = getTypeAsString(installed);
    if (!availableTypes.isEmpty() && !installed )
        result += "   ( found =" + getTypeAsString() + ")";
    else if (!availableTypes.isEmpty() && installed )
        result += "   ( installed =" + getTypeAsString() + ")";
    return result;
}

QString Package::getTypeAsString(bool requiredIsInstalled, const QString &delim) const
{
    QString types;

    QHash<FileTypes::Type, PackageItem>::ConstIterator it = m_packages.constBegin();
    for( ; it != m_packages.constEnd(); ++it) {
        if(requiredIsInstalled && !(*it).installed())
            continue;
        types += FileTypes::toString((*it).contentType())  + delim;
    }
    return types;
}

bool Package::isInstalled(FileTypes::Type contentType) const
{
    if (contentType == FileTypes::ANY)
    {
        Q_FOREACH (const PackageItem &item, m_packages)
        {
            if (item.installed())
                return true;
        }
        return false;
    }
    if(m_packages.contains(contentType))
        return m_packages[contentType].installed();
    return false;
}

void Package::setInstalled(FileTypes::Type contentType)
{
    if(m_packages.contains(contentType))
        m_packages[contentType].setInstalled(true);
}

// deprecated?
bool Package::write(QTextStream &out) const
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << m_name << "\t" << m_version << "\t" << getTypeAsString(true,"\t") << "\n";
#endif
    out << m_name << "\t" << m_version.toString()
// FIXME store path relocation information for removing too
    << "\t"
    << (isInstalled(FileTypes::BIN) ? "bin:" : ":")
    << (isInstalled(FileTypes::LIB) ? "lib:" : ":")
    << (isInstalled(FileTypes::DOC) ? "doc:" : ":")
    << (isInstalled(FileTypes::SRC) ? "src:" : ":")
    << (isInstalled(FileTypes::DBG) ? "dbg:" : QString())
    << ";"
    << ";"
    << getUrl(FileTypes::BIN).toString() << ";"
    << getUrl(FileTypes::LIB).toString() << ";"
    << getUrl(FileTypes::DOC).toString() << ";"
    << getUrl(FileTypes::SRC).toString() << ";"
    << getUrl(FileTypes::DBG).toString()
    << "\n";
    return true;
}

// deprecated?
bool Package::read(QTextStream &in)
{
    if (in.atEnd())
        return false;
    QString line = in.readLine();
    const QStringList parts = line.split('\t');
    setName(parts.at(0));
    setVersion(parts.at(1));
    const QStringList options = parts.at(2).split(';');
    const QStringList state = options.at(0).split(':');
    const QString &baseURL = options.at(1);
    if(!options.at(2).isEmpty())
    {
        Package::PackageItem item(FileTypes::BIN);
        item.setInstalled(state.size() > 0 && state.at(0) == "bin");
        if (item.setUrlAndFileName(options.at(2), ""))
          add(item);
        else
            qWarning() << "could not set BIN packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(3).isEmpty())
    {
        Package::PackageItem item(FileTypes::LIB);
        item.setInstalled(state.size() > 1 && state.at(1) == "lib");
        if (item.setUrlAndFileName(options.at(3), ""))
          add(item);
        else
            qWarning() << "could not set LIB packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(4).isEmpty())
    {
        Package::PackageItem item(FileTypes::DOC);
        item.setInstalled(state.size() > 2 && state.at(2) == "doc");
        if (item.setUrlAndFileName(options.at(4), ""))
            add(item);
        else
            qWarning() << "could not set DOC packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(5).isEmpty())
    {
        Package::PackageItem item(FileTypes::SRC);
        item.setInstalled(state.size() > 3 && state.at(3) == "doc");
        if (item.setUrlAndFileName(options.at(5), ""))
            add(item);
        else
            qWarning() << "could not set SRC packageitem for package '" << parts.at(0) << "'";
    }
    return true;
}

QString Package::localFilePath(FileTypes::Type type, bool bCreateDir)
{
    QString dir = Settings::instance().downloadDir();
    QDir d(dir);

    if(bCreateDir) {
        if(!d.exists(".")) {
            // FIXME: can't we use d.mkpath(dir) ?
            if(!QDir().mkpath(dir)) {
                qDebug() << "Can't create directory " << dir;
            }
        }
    }
    return d.absoluteFilePath(localFileName(type));
}

static QByteArray readMD5SumFile(const QString &filename)
{
  QFile f(filename);
  if(!f.open(QIODevice::ReadOnly))
    return QByteArray();
  // simply use the first 32 Bytes - make this more robust!
  QByteArray str = f.readLine().trimmed().left(32);
  if(str.length() != 32)
    return QByteArray();
  return str;
}

bool Package::downloadItem(FileTypes::Type type)
{
    QUrl url = getUrl(type);
    qDebug() << __FUNCTION__ << " going to download URL " << url.toString() << "type" << type;

    if (url.isEmpty()) {
        qWarning() << __FUNCTION__ << " empty URL ";
        return setError(QObject::tr("server configuration error - invalid download URL %1").arg(url.toString()));
    }

    int ret = 0; 
    QString fn = localFilePath(type, true);
    /*
    There are three modes of checksum sum handling, which are handled in the following order: 
        case 1. checksum checking is disabled, 
            in this case only the archive is downloaded into the download directory if not present
            and no further checksum checking is performed 
    */
    QString archiveDownloadError = QObject::tr("could not download archive %1").arg(url.toString());
    QString download2Error = QObject::tr("downloaded archive %1 not accessable").arg(fn);
    QString checkSumError = QObject::tr("archive downloaded from %1 checksum error").arg(url.toString());

    if (m_hashType.type() == Hash::None)
    {
        //  if archive is not present download it
        if (!QFile::exists(fn)) 
        {
            qDebug() << __FUNCTION__ << " downloading archive";
            ret = Downloader::instance()->fetch(url, fn);
            if (!ret || Downloader::instance()->result() != Downloader::Finished)
                return setError(archiveDownloadError); 
        }
        if (QFile::exists(fn))
            return true;
        else
            return setError(download2Error); 
    }

    // keep download checksum generating in sync with package
    Downloader::instance()->setCheckSumType(m_hashType.type());
    QByteArray archiveCheckSum;

    /*
    case 2. an md5 or sha1 checksum is provided in the central package configuration
            In this case the archive is downloaded into the download directory if not present,  
            the checksum of the package will be computed and compared. If the checksum does not fit 
            redownload the archive and check the checksum again. If the checksum still does 
            not fit delete the archive file and reports an checksum file error to the user. 
    */
    if (!item(type).checkSum().isEmpty()) 
    {
        //  if archive is not present download it
        if (!QFile::exists(fn)) 
        {
            qDebug() << __FUNCTION__ << " downloading archive";
            ret = Downloader::instance()->fetch(url, fn);
            if (!ret || Downloader::instance()->result() != Downloader::Finished)
                return setError(archiveDownloadError);
            archiveCheckSum = Downloader::instance()->checkSum().toHex();
        }
        else 
            archiveCheckSum = m_hashType.hash(fn).toHex();

        if( archiveCheckSum == item(type).checkSum()) 
            return true;

        QFile::remove(fn);
        ret = Downloader::instance()->fetch(url, fn);
        if (!ret || Downloader::instance()->result() != Downloader::Finished)
            return setError(archiveDownloadError); 
        archiveCheckSum = Downloader::instance()->checkSum().toHex();

        if( archiveCheckSum == item(type).checkSum() ) 
            return true;

        qCritical() << __FUNCTION__ << "could not compute checksum after two tries";
        return setError(checkSumError); 
    }
    
    /*
    case 3. on the mirror there are checksum files for the related package. Here 
            1. download the checksum file into the download dir if not present 
            2. download the archive into the download dir if not present 
            3. validate archive by the given hash file
            4. if validation fails for the first time goto 1  else report error
    */
    HashFile hashFile(m_hashType.type(),fn);
    QString hashFileName = fn + hashFile.fileNameExtension();
    QUrl hashUrl = url;
    hashUrl.setPath(url.path() + hashFile.fileNameExtension());
    QString hashDownloadError = QObject::tr("could not download archive integrity file from %1").arg(hashUrl.toString());
    QString hashFileError = QObject::tr("could not open archive integrity file %1").arg(hashFileName);

    // the checksum sum may not be present from older downloads, try downloading first. 
    // This prevents redownloading the archive unconditionally when no checksum is present 
    if (!QFile::exists(hashFileName)) {
        qDebug() << __FUNCTION__ << " downloading checksum file";
        ret = Downloader::instance()->fetch(hashUrl, hashFileName);
        if (!ret || Downloader::instance()->result() != Downloader::Finished)
            return setError(hashDownloadError); 
    }
    if (!hashFile.readFromFile(hashFileName))
        return setError(hashFileError); 
    
    //  if archive is not present download it
    if (!QFile::exists(fn)) 
    {
        qDebug() << __FUNCTION__ << " downloading package file";
        ret = Downloader::instance()->fetch(url, fn);
        if (!ret || Downloader::instance()->result() != Downloader::Finished)
            return setError(archiveDownloadError); 
        archiveCheckSum = Downloader::instance()->checkSum().toHex();
    }
    else    
        archiveCheckSum = m_hashType.hash( fn ).toHex();
 
    if( hashFile.getHash() == archiveCheckSum ) {
        qDebug() << __FUNCTION__ << "checksum is correct - no need to redownload file";
        return true;
    }

    qDebug() << __FUNCTION__ << "checksum is not correct - need to download file again!";
    QFile::remove(fn);
    QFile::remove(hashFileName);

    qDebug() << __FUNCTION__ << " downloading checksum file";
    Downloader::instance()->fetch(hashUrl, hashFileName);
    if (!ret || Downloader::instance()->result() != Downloader::Finished)
        return setError(archiveDownloadError); 

    if (!hashFile.readFromFile(hashFileName))
        return setError(hashFileError); 

    qDebug() << __FUNCTION__ << " downloading package file";
    ret = Downloader::instance()->fetch(url, fn);
    if (!ret || Downloader::instance()->result() != Downloader::Finished)
        return setError(archiveDownloadError); 
    archiveCheckSum = Downloader::instance()->checkSum().toHex();

    if(hashFile.getHash() == archiveCheckSum) {
        qDebug() << __FUNCTION__ << "md5sum is correct - no need to redownload file";
        return true;
    }
    qCritical() << __FUNCTION__ << "could not verify checksum of archive after two tries";
    return setError(checkSumError); 
}

bool Package::installItem(Installer *installer, FileTypes::Type type)
{
    QString fileName = localFileName(type);
    if (fileName.isEmpty())
    {
#ifdef DEBUG
        qDebug() << __FUNCTION__ << " empty fileName for type " << type;
#endif
        return false;
    }
    if (!installer->install(this, type))
    {
        qDebug() << __FUNCTION__ << " install failure for file " << localFilePath(type) << " type " << type;
        return false;
    }

    setInstalled(type);

    return true;
}

bool Package::removeItem(Installer *installer, FileTypes::Type type)
{
    QString manifestFile = installer->root()+"/manifest/"+PackageInfo::manifestFileName(name(),installedVersion().toString(),type);
    installer->uninstall(manifestFile);
    return true;
}

void Package::addCategories(const QString &cat)
{
    if (cat.contains(QLatin1Char(' ')))
    {
        Q_FOREACH(const QString &acat, cat.split(QLatin1Char(' ')))
        {
            if (!m_categories.contains(acat))
                m_categories << acat;
        }
    }
    else if (!m_categories.contains(cat))
        m_categories << cat;
}

void Package::addCategories(const QStringList &cats)
{
    Q_FOREACH(const QString &cat, cats)
        addCategories(cat);
}

void Package::addDeps(const QStringList &deps)
{
    Q_FOREACH(const QString &dep, deps)
    {
        if (dep.contains(QLatin1Char(' ')))
        {
            Q_FOREACH(const QString &adep, dep.split(QLatin1Char(' '), QString::SkipEmptyParts))
            {
                if (!m_deps.contains(adep))
                    m_deps << adep;
            }
        }
        else if (!m_deps.contains(dep))
            m_deps << dep;
    }
}
#endif

QString Package::manifestFileName(const FileTypes::Type type) const
{
    return PackageInfo::manifestFileName(m_name,m_version.toString(),type);
}

QString Package::versionFileName(const FileTypes::Type type) const
{
    return PackageInfo::versionFileName(m_name,m_version.toString(),type);
}

QDebug &operator<<(QDebug &out, const Package &c)
{
    out << "Package ("
        << "name:" << c.name()
        << "version:" << c.m_version.toString()
        << "installedversion:" << c.installedVersion()
        << "categories:" << c.categories()
        << "notes:" << c.notes()
        << "longNotes:" << c.longNotes()
        << "deps:" << c.deps()
        << " m_pathRelocs: ";

    StringHash::ConstIterator its = c.m_pathRelocs.constBegin();
    for( ; its != c.m_pathRelocs.constEnd(); its++)
        out << its.key() + "=" + its.value() ;

    Package::PackageItemType::ConstIterator it = c.m_packages.constBegin();
    for( ; it != c.m_packages.constEnd(); ++it)
        out << *it;
    out << ")";
    return out;
}


QDebug &operator<<(QDebug &out, const QList<Package*> &c)
{
    out << "QList<Package *> ("
        << "size:" << c.size();
    Q_FOREACH(const Package *p, c)
        out << *p;
    out << ")";
    return out;
}
