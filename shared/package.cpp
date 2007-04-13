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

#include <QtDebug>
#include <QStringList>
#include <QFile>
#include <QDir>

#include "package.h"
#include "downloader.h"
#include "installer.h"
#include "database.h"
#include "uninstall.h"


bool PackageInfo::fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat)
{
    QString baseName; 

    // first remove ending
    int idx = fileName.lastIndexOf('.');
    if(idx != -1)
    {
        pkgFormat = fileName.mid(idx + 1);
        baseName = fileName.left(idx).toLower();
    }
    else
    {
        pkgFormat = "unknown";
        baseName = fileName.toLower();
    }

    QStringList parts = baseName.split('-');

    if (parts.size() == 5)
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
        qDebug() << "can't parse filename " << baseName;
        return false;
    }
    else     
    {
        qDebug() << __FUNCTION__ << "unhandled case with" << baseName;
        return false;
    }
    return true;
}


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

void Package::PackageItem::dump(const QString &title) const
{
    DUMP_HEADER(title,"PackageItem");
    qDebug() << "url:         " << url;           
    qDebug() << "fileName:    " << fileName;       
    qDebug() << "packageType: " << packageType;    
    qDebug() << "contentType: " << contentType;    
    qDebug() << "bInstalled:  " << bInstalled;     
    DUMP_FOOTER(title,"PackageItem");
}

Package::Package()
{
    m_handled = false;
}

Package::Package(const Package &other)
{
    m_packages   = other.m_packages;
    m_name       = other.m_name;
    m_version    = other.m_version;
    m_category   = other.m_category;
    m_deps       = other.m_deps;
    m_pathRelocs = other.m_pathRelocs;
    m_handled    = other.m_handled;
    m_notes      = other.m_notes;
}

QString Package::getFileName(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].fileName;
    return QString();
}

QString Package::getURL(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].url;
    return QString();
}

QString Package::getBaseURL()
{
    // FIXME: obsolate remove it
    return QString();
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


QString Package::toString(bool installed, const QString &delim)
{
    QString result = m_name + delim + m_version;
    QString availableTypes = getTypeAsString(installed);
    if (!availableTypes.isEmpty() && !installed )
        result += "   ( found =" + getTypeAsString() + ")";
    else if (!availableTypes.isEmpty() && installed )
        result += "   ( installed =" + getTypeAsString() + ")";
    return result;
}

QString Package::getTypeAsString(bool requiredIsInstalled, const QString &delim)
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
        };
    }
    return types;
}

bool Package::isInstalled(Package::Type contentType) const
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].bInstalled;
    return false;
}

void Package::setInstalled(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        m_packages[contentType].bInstalled = true;
}

bool Package::write(QTextStream &out)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << m_name << "\t" << m_version << "\t" << getTypeAsString(true,"\t") << "\n";
#endif
    QString baseUrl = getBaseURL();

    out << m_name << "\t" << m_version 
// FIXME store path relocation information for removing too
    << "\t" 
    << (isInstalled(BIN) ? "bin:" : ":")
    << (isInstalled(LIB) ? "lib:" : ":")
    << (isInstalled(DOC) ? "doc:" : ":")
    << (isInstalled(SRC) ? "src" : QString())
    << ";"
    << baseUrl << ";"
    << (baseUrl.isEmpty() ? getURL(BIN) + '/' : QString()) + getFileName(BIN) << ";"
    << (baseUrl.isEmpty() ? getURL(LIB) + '/' : QString()) + getFileName(LIB) << ";"
    << (baseUrl.isEmpty() ? getURL(DOC) + '/' : QString()) + getFileName(DOC) << ";"
    << (baseUrl.isEmpty() ? getURL(SRC) + '/' : QString()) + getFileName(SRC)
    << "\n";
    return true;
}

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
        item.set(baseURL, options.at(2), BIN, state.size() > 0 && state.at(0) == "bin");
        add(item);
    }
    if(!options.at(3).isEmpty())
    {    
        Package::PackageItem item;
        item.set(baseURL, options.at(3), LIB, state.size() > 1 && state.at(1) == "lib");
        add(item);
    }
    if(!options.at(4).isEmpty())
    {    
        Package::PackageItem item;
        item.set(baseURL, options.at(4), DOC, state.size() > 2 && state.at(2) == "doc");
        add(item);
    }
    if(!options.at(5).isEmpty())
    {    
        Package::PackageItem item;
        item.set(baseURL, options.at(5), SRC, state.size() > 3 && state.at(3) == "src");
        add(item);
    }
    return true;
}

void Package::dump(const QString &title) const
{
    DUMP_HEADER(title,"Package");
    qDebug() << "m_name:    " << m_name;
    qDebug() << "m_version: " << m_version;
    qDebug() << "m_category: " << m_category;
    qDebug() << "m_deps: " << m_deps.join(" ");
    qDebug() << "m_handled: " << m_handled;

    QString d;
    StringHash::ConstIterator its = m_pathRelocs.constBegin();
    for( ; its != m_pathRelocs.constEnd(); its++) 
    {
        d += its.key() + " = " + its.value() + ' ';
    }
    qDebug() << "m_pathRelocs: " << d;

    QHash<Type, PackageItem>::ConstIterator it = m_packages.constBegin();
    for( ; it != m_packages.constEnd(); ++it) {
        it->dump();
    }
    DUMP_FOOTER(title,"Package");
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
    QString dir = Settings::getInstance().downloadDir();
    if (Settings::getInstance().nestedDownloadTree())
        dir += '/' + m_name + '/' + m_name + '-' + m_version + '/';
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

bool Package::downloadItem(Downloader *downloader, Package::Type type)
{
    QString URL = getURL(type);
    if (URL.isEmpty()) {
        qDebug() << __FUNCTION__ << " empty URL for type " << type;
        return false;
    }
    QString fn = makeFileName(type, true);
    if(QFile::exists(fn)) {
        qDebug() << __FUNCTION__ << " URL " << URL << " already downloaded for type " << type;
        return false; 
    }
    qDebug() << __FUNCTION__ << " downloading URL " << URL << " for type " << type;
    return downloader->start(URL, fn);
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
    if (!installer->install(fileName, pathRelocations())) 
    {
        qDebug() << __FUNCTION__ << " install failure for file " << fileName << " type " << type;
        return false;
    }
    
    setInstalled(type);

    return true;
}

bool Package::removeItem(Installer *installer, Package::Type type)
{
    QString manifestFile = installer->root()+"/manifest/"+ installer->database()->manifestFileName(name(),version(),type);
    Uninstall ui(installer->root(),manifestFile);
    ui.uninstallPackage(false);
    return true;
}

void Package::logOutput()
{}

void Package::setCategory(const QString &cat)
{
    m_category = cat;
}

void Package::addDeps(const QStringList &deps)
{
    m_deps << deps;
}

/** 
  initiate package item 
  @param url url for downloading 
  @param fn local package item filename 
  @param contentType package item content type as string 
  @param bInstalled flag if package item is installed
 */ 
bool Package::PackageItem::set(const QString &url, const QString &fn, const QByteArray &contentType, bool bInstalled)
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
bool Package::PackageItem::set(const QString &url, const QString &fn, Package::Type contentType, bool bInstalled)
{
    PackageItem desc;
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << path << fn << contentType << bInstalled;
#endif
    desc.url = url;
    // generate fileName from from url 
    if(fn.isEmpty()) 
    {
        idx = url.lastIndexOf('/');
        if(idx == -1) {
            qDebug() << __FUNCTION__ << "Invalid - no complete url found " << url;    // FIXME
            return false;
        }
        desc.fileName = url.mid(idx + 1);
    }
    else
        desc.fileName = fn;

    idx = desc.fileName.lastIndexOf('.');
    if(idx == -1) {
        qDebug() << __FUNCTION__ << "Invalid - dot in filename expected" << desc.fileName;    // FIXME
        return false;
    }
    desc.packageType = desc.fileName.mid(idx + 1);
    
    desc.contentType = contentType;
    desc.bInstalled  = bInstalled;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << desc.contentType << desc.bInstalled;
#endif    
    *this = desc;
    return true;
}
