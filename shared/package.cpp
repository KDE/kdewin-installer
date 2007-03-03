/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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

#include "config.h"

#include <QtDebug>
#include <QStringList>
#include <QFile>
#include <QDir>

#include "package.h"
#include "downloader.h"
#include "installer.h"

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
    qDebug() << "path:        " << path;           
    qDebug() << "fileName:    " << fileName;       
    qDebug() << "packageType: " << packageType;    
    qDebug() << "contentType: " << contentType;    
    qDebug() << "bInstalled:  " << bInstalled;     
    DUMP_FOOTER(title,"PackageItem");
}

Package::Package()
{}

Package::Package(const Package &other)
{
    m_packages   = other.m_packages;
    m_name       = other.m_name;
    m_version    = other.m_version;
    m_category   = other.m_category;
    m_deps       = other.m_deps;
    m_pathRelocs = other.m_pathRelocs;
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
        return m_packages[contentType].path + '/' + m_packages[contentType].fileName;
    return QString();
}

QString Package::getBaseURL()
{
    QString baseUrl;
    QHash<Type, PackageItem>::ConstIterator it = m_packages.constBegin();
    for( ; it != m_packages.constEnd(); ++it) {
        if(baseUrl.isEmpty()) {
            baseUrl = (*it).path;
        } else {
            if((*it).path != baseUrl)
                return QString();
        }
    }
    return baseUrl;
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
        qDebug() << __FUNCTION__ << " empty fileName for type " << type;
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

bool Package::setFromVersionFile(const QString &str)
{
    QString verString = str;
    verString.remove(".ver");
    int i = verString.indexOf('-');
    int j = verString.lastIndexOf('-');
    QString name = verString.left(i);
    QString version = verString.mid(i+1,j-1-i);
    QString type = verString.mid(j+1);
    setName(name);
    setVersion(version);
//    setType(type);
    return true;
}

bool Package::PackageItem::set(const QString &path, const QString &fn, const QByteArray &contentType, bool bInstalled)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << " " << path << " " << contentType << " " << bInstalled;
#endif
    QByteArray ct = contentType.toLower();
    if(ct == "bin")
        return set(path, fn, BIN, bInstalled);
    else
    if(ct == "lib")
        return set(path, fn, LIB, bInstalled);
    else
    if(ct == "doc")
        return set(path, fn, DOC, bInstalled);
    else
    if(ct == "src")
        return set(path, fn, SRC, bInstalled);
    // unknown type 
    return false;
}

bool Package::PackageItem::set(const QString &path, const QString &fn, Package::Type contentType, bool bInstalled)
{
    PackageItem desc;
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << path << fn << contentType << bInstalled;
#endif
    // FIXME:  fn=="/" is an unwanted condition 
    if(fn.isEmpty() || fn == "/" && path.isEmpty()) 
        return false;

    if(path.isEmpty()) {
        // every package has different baseUrl
        idx = fn.lastIndexOf('/');
        if(idx == -1) {
            qDebug() << __FUNCTION__ << "Parser error! - no '/' in path";    // FIXME
            return false;
        }
        desc.path = fn.left(idx);
        desc.fileName = fn.mid(idx + 1);
    } else {
        // every package has same baseUrl
        desc.path = path;
        desc.fileName = fn;
    }

    idx = desc.fileName.lastIndexOf('.');
    if(idx == -1) {
        qDebug() << "Invalid - dot in filename expected" << desc.fileName;    // FIXME
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
