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

#include "config.h"
#include "package.h"
#include "downloader.h"
#include "installer.h"
#include "database.h"
#include "uninstaller.h"

#include <QtDebug>
#include <QStringList>
#include <QFile>
#include <QDir>

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
	return compare(other) < 0 ? true : false; 
}

bool Package::PackageVersion::operator>(const Package::PackageVersion &other) const
{
	return compare(other) > 0; 
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
    QByteArray ct = contentType.toLower();
    if(ct == "bin")
        return set(url, fn, BIN, bInstalled);
    else
    if(ct == "lib")
        return set(url, fn, LIB, bInstalled);
    else
    if(ct == "doc")
        return set(url, fn, DOC, bInstalled);
    else
    if(ct == "src")
        return set(url, fn, SRC, bInstalled);
    // unknown type
    return false;
}

/**
  initiate package item
  @param url url for downloading
  @param fn local package item filename
  @param contentType package item content type
  @param bInstalled flag if package item is installed
 */
bool Package::PackageItem::set(const QUrl &url, const QString &fn, Package::Type contentType, bool bInstalled)
{
    PackageItem desc;
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << path << fn << contentType << bInstalled;
#endif
    // generate fileName from from url
    if(fn.isEmpty())
    {
        QString sUrl = url.toString();
        idx = sUrl.lastIndexOf('/');
        if(idx == -1) {
            qDebug() << __FUNCTION__ << "Invalid - no complete url found " << url << "packageitem ignored";    // FIXME
            return false;
        }
        desc.fileName = sUrl.mid(idx + 1);
    }
    else
        desc.fileName = fn;

    idx = desc.fileName.lastIndexOf('.');
    if(idx == -1) {
        qDebug() << __FUNCTION__ << "Invalid - dot in filename expected" << desc.fileName;    // FIXME
        return false;
    }
    desc.packageType = desc.fileName.mid(idx + 1);

    desc.url = url;
    desc.contentType = contentType;
    desc.bInstalled  = bInstalled;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << desc.contentType << desc.bInstalled;
#endif
    *this = desc;
    return true;
}

bool Package::PackageItem::setContentType(const QString &type)
{
    QString ct = type.toLower();
    if(ct == "bin")
    {
        contentType = BIN;
        return true;
    }
    else if(ct == "lib")
    {
        contentType = LIB;
        return true;
    }
    else if(ct == "doc")
    {
        contentType = DOC;
        return true;
    }
    else if(ct == "src")
    {
        contentType = SRC;
        return true;
    }
    else
    {
        contentType = NONE;
        return false;
    }
}
#endif

static QString typeToString(Package::Type type)
{
    switch(type) {
        case Package::BIN:   return "BIN";
        case Package::LIB:   return "LIB";
        case Package::DOC:   return "DOC";
        case Package::SRC:   return "SRC";
        default: return "unknown" + QString::number(type);
    }
}
QDebug &operator<<(QDebug &out, const Package::Type c)
{
    switch(c) {
        case Package::BIN:   out << "BIN";  break;
        case Package::LIB:   out << "LIB";  break;
        case Package::DOC:   out << "DOC";  break;
        case Package::SRC:   out << "SRC";  break;
        default: out << "unknown" + QString::number(c);
    }
    return out;
}

QDebug &operator<<(QDebug &out, const Package::PackageItem &c)
{
    out << "PackageItem ("
        << "url:" << c.url
        << "fileName:" << c.fileName
        << "packageType:" << c.packageType
        << "contentType:" << typeToString(c.contentType)
        << "bInstalled:" << c.bInstalled
        << ")";
    return out;
}

#ifndef PACKAGE_SMALL_VERSION

Package::Package()
{
    m_handled = false;
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
    m_installedversion = other.m_installedversion;
}

QString Package::getFileName(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].fileName;
    return QString();
}

QUrl Package::getUrl(Package::Type contentType) const
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].url;
    return QString();
}

bool Package::setUrl(Package::Type contentType, const QUrl &url)
{
    if(!m_packages.contains(contentType))
        return false;

    m_packages[contentType].url = url;
    return true;
}

bool Package::add(const PackageItem &item)
{
    if(m_packages.contains(item.contentType)) {
        qDebug() << __FUNCTION__ << m_name << " type " << item.contentType << "already added";
        return false;
    }
    m_packages[item.contentType] = item;
    return true;
}

void Package::setInstalled(const Package &other)
{
    // FIXME!
}

bool Package::hasType(Package::Type contentType) const
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

    QHash<Type, PackageItem>::ConstIterator it = m_packages.constBegin();
    for( ; it != m_packages.constEnd(); ++it) {
        if(requiredIsInstalled && !(*it).bInstalled)
            continue;

        switch((*it).contentType) {
            case BIN:
                types += "bin" + delim;
                break;
            case LIB:
                types += "lib" + delim;
                break;
            case DOC:
                types += "doc" + delim;
                break;
            case SRC:
                types += "src" + delim;
                break;
          default:
                break;
        };
    }
    return types;
}

bool Package::isInstalled(Package::Type contentType) const
{
    if (contentType == ANY)
    {
        Q_FOREACH (const PackageItem &item, m_packages)
        {
            if (item.bInstalled)
                return true;
        }
        return false;
    }
    if(m_packages.contains(contentType))
        return m_packages[contentType].bInstalled;
    return false;
}

void Package::setInstalled(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        m_packages[contentType].bInstalled = true;
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
    << (isInstalled(BIN) ? "bin:" : ":")
    << (isInstalled(LIB) ? "lib:" : ":")
    << (isInstalled(DOC) ? "doc:" : ":")
    << (isInstalled(SRC) ? "src" : QString())
    << ";"
    << ";"
    << getUrl(BIN).toString() << ";"
    << getUrl(LIB).toString() << ";"
    << getUrl(DOC).toString() << ";"
    << getUrl(SRC).toString()
    << "\n";
    return true;
}

// deprecated?
bool Package::read(QTextStream &in)
{
    if (in.atEnd())
        return false;
    QString line = in.readLine();
    QStringList parts = line.split('\t');
    setName(parts.at(0));
    setVersion(parts.at(1));
    QStringList options = parts.at(2).split(';');
    QStringList state = options.at(0).split(':');
    QString baseURL = options.at(1);
    if(!options.at(2).isEmpty())
    {
        Package::PackageItem item;
        if (item.set(options.at(2), "", BIN, state.size() > 0 && state.at(0) == "bin"))
          add(item);
        else
            qWarning() << "could not set BIN packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(3).isEmpty())
    {
        Package::PackageItem item;
        if (item.set(options.at(3), "", LIB, state.size() > 1 && state.at(1) == "lib"))
          add(item);
        else
            qWarning() << "could not set LIB packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(4).isEmpty())
    {
        Package::PackageItem item;
        if (item.set(options.at(4), "", DOC, state.size() > 2 && state.at(2) == "doc"))
            add(item);
        else
            qWarning() << "could not set DOC packageitem for package '" << parts.at(0) << "'";
    }
    if(!options.at(5).isEmpty())
    {
        Package::PackageItem item;
        if (item.set(options.at(5), "", SRC, state.size() > 3 && state.at(3) == "src"))
            add(item);
        else
            qWarning() << "could not set SRC packageitem for package '" << parts.at(0) << "'";
    }
    return true;
}

static bool makeDir(const QDir &dir)
{
    if(dir.exists())
        return true;
    if(dir.isRoot())
        return false;
    QString d = dir.absolutePath();
    int idx = d.lastIndexOf('/');
    if(idx == -1)
        return false;
    if(makeDir(QDir(d.left(idx))))
        return dir.mkdir(d);
    return false;
}

QString Package::makeFileName(Package::Type type, bool bCreateDir)
{
    QString dir = Settings::instance().downloadDir();
    QDir d(dir);

    if(bCreateDir) {
        if(!d.exists(".")) {
            if(!makeDir(d)) {
                qDebug() << "Can't create directory " << dir;
            }
        }
    }
    return d.absoluteFilePath(getFileName(type));
}

bool Package::downloadItem(Package::Type type)
{
    QUrl url = getUrl(type);
    if (url.isEmpty()) {
        qDebug() << __FUNCTION__ << " empty URL for type " << type;
        return false;
    }
    QString fn = makeFileName(type, true);
    if(QFile::exists(fn)) {
        qDebug() << __FUNCTION__ << " URL " << url.toString() << " already downloaded for type " << type;
        return true;
    }
    qDebug() << __FUNCTION__ << " downloading URL " << url.toString() << " for type " << type;
    return Downloader::instance()->start(url, fn);
}

bool Package::installItem(Installer *installer, Package::Type type)
{
    QString fileName = getFileName(type);
    if (fileName.isEmpty())
    {
#ifdef DEBUG
        qDebug() << __FUNCTION__ << " empty fileName for type " << type;
#endif
        return false;
    }
    fileName = makeFileName(type);
    if (!installer->install(this, type, fileName))
    {
        qDebug() << __FUNCTION__ << " install failure for file " << fileName << " type " << type;
        return false;
    }

    setInstalled(type);

    return true;
}

bool Package::removeItem(Installer *installer, Package::Type type)
{
    QString manifestFile = installer->root()+"/manifest/"+Package::manifestFileName(name(),installedVersion().toString(),type);
    installer->uninstall(manifestFile);
    return true;
}

void Package::logOutput()
{}

void Package::addCategories(const QString &cat)
{
    if (cat.contains(QLatin1Char(' ')))
    {
        Q_FOREACH(const QString &acat, cat.split(QLatin1Char(' ')))
            m_categories << acat;
    }
    else
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

QString Package::typeToString(Package::Type type)
{
    switch(type) {
        case BIN:    return "bin";
        case LIB:    return "lib";
        case DOC:    return "doc";
        case SRC:    return "src";
        case ALL:    return "all";
        default:    return "unknown";
    }
}

bool Package::fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat)
{
    QString baseName;

    // first remove ending
    int idx  = fileName.lastIndexOf('.');
    if(idx != -1)
    {
        pkgFormat = fileName.mid(idx + 1);
        baseName = fileName.left(idx).toLower();
        if(pkgFormat == "bz2") {
            int idx2 = fileName.lastIndexOf('.', idx - 1);
            pkgFormat = fileName.mid(idx2 + 1);
            idx = idx2;
        }
        baseName = fileName.left(idx).toLower();
    }
    else
    {
        pkgFormat = "unknown";
        baseName = fileName.toLower();
    }

    QStringList parts = baseName.split('-');

    if (parts.size() == 6)
    {
        // a-b-c-version-patchlevel-type
        pkgName = parts[0] + "-" + parts[1] + "-" + parts[2];
        pkgVersion = parts[3] + "-" + parts[4];
        pkgType = parts[5];
    }
    else if (parts.size() == 5)
    {
        // a-b-c-version-type
        if (parts[2][0].isLetter())
        {
            pkgName = parts[0] + "-" + parts[1] + "-" + parts[2];
            pkgVersion = parts[3];
        }
        else
        {
            pkgName = parts[0] + "-" + parts[1];
            pkgVersion = parts[2] + '-' + parts[3];
        }
        pkgType = parts[4];
    }
    else if (parts.size() == 4)
    {
        // a-b-version-x
        if (parts[1][0].isLetter())
        {
            pkgName += parts[0] + "-" + parts[1];
            pkgVersion = parts[2];
        }
        else
        {
            pkgName = parts[0];
            pkgVersion = parts[1] + '-' + parts[2];
        }
        // a-b-version-patch
        if (parts[3][0].isNumber())
        {
            pkgType = "all";
            pkgVersion += "-" + parts[3];
        }
        else
            pkgType = parts[3];
    }
    else if(parts.size() == 3)
    {
        pkgName = parts[0];
        if (parts[1][0].isNumber())
            pkgVersion = parts[1];
        else
            pkgName += "-" + parts[1];

        // aspell-0.50.3-3
        if (parts[2][0].isNumber())
        {
            if (pkgVersion.isEmpty())
                pkgVersion = parts[2];
            else
                pkgVersion += "-" + parts[2];
            pkgType = "bin";
        }
        else
            pkgType = parts[2];
    }
    else if(parts.size() == 2)
    {
        pkgName = parts[0];
        pkgVersion = parts[1];
        pkgType = "bin";
    }
    else if(parts.size() < 2)
    {
        qWarning() << "can't parse filename " << baseName;
        return false;
    }
    else
    {
        qWarning() << __FUNCTION__ << "unhandled case with" << baseName;
        return false;
    }
    return true;
}

bool Package::fromString(const QString &name, QString &pkgName, QString &pkgVersion)
{
    QStringList parts = name.split('-');
    // <name>-<version>
    if (parts.size() == 2 && parts[1][0].isNumber())
    {
        pkgName = parts[0];
        pkgVersion = parts[1];
        return true;
    }
    // <name>-<version>-<patchlevel>
    else if (parts.size() == 3 && parts[1][0].isNumber())
    {
        pkgName = parts[0];
        pkgVersion = parts[1] + '-' + parts[2];
        return true;
    }
    // <name1>-<name2>-<version>
    else if (parts.size() == 3 && parts[2][0].isNumber())
    {
        pkgName = parts[0] + '-' + parts[1];
        pkgVersion = parts[2];
        return true;
    }
    else
        return false;
}

QString Package::manifestFileName(const Package::Type type)
{
    return Package::manifestFileName(m_name,m_version.toString(),type);
}

QString Package::versionFileName(const Package::Type type)
{
    return Package::versionFileName(m_name,m_version.toString(),type);
}

// returns version file name of package item e.g. xyz-1.2.3-bin.ver
QString Package::versionFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + Package::typeToString(type) +".ver";
}

// returns manifest file name of package item e.g. xyz-1.2.3-bin.mft
QString Package::manifestFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + Package::typeToString(type) +".mft";
}

QDebug &operator<<(QDebug &out, const Package &c)
{
    out << "Package ("
        << "name:" << c.name()
        << "version:" << c.m_version.toString()
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



