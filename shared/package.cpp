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

#include <QtDebug>
#include <QStringList>
#include <QFile>

#include "package.h"
#include "downloader.h"
#include "installer.h"

void Package::PackageItem::dump(const QString &title) const
{
    qDebug() << "class packageDesc dump: " << title;
    qDebug() << "path:        " << path;           
    qDebug() << "fileName:    " << fileName;       
    qDebug() << "packageType: " << packageType;    
    qDebug() << "contentType: " << contentType;    
    qDebug() << "bInstalled:  " << bInstalled;     
}

// FIXME: this should be in PackageList to have access from Package, 
// Package class should store PackageList pointer as parent
QString Package::baseURL = "http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/";

Package::Package()
{}

QString Package::getFileName(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].fileName;
    return QString();
}

QString Package::getURL(Package::Type contentType)
{
    if(m_packages.contains(contentType))
        return m_packages[contentType].path;
    return QString();
}

void Package::add(const QString &path, const QByteArray &contentType, bool bInstalled)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << " " << path << " " << contentType << " " << bInstalled;
#endif
    QByteArray ct = contentType.toLower();
    if(ct == "bin")
        add(path, BIN, bInstalled);
    else
    if(ct == "lib")
        add(path, LIB, bInstalled);
    else
    if(ct == "doc")
        add(path, DOC, bInstalled);
    else
    if(ct == "src")
        add(path, SRC, bInstalled);
}

void Package::add(const QString &path, Package::Type contentType, bool bInstalled)
{
    PackageItem desc;
    int idx;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << " " << path << " " << contentType << " " << bInstalled;
#endif
    desc.path = path;

    idx = path.lastIndexOf('/');
    if(idx == -1) {
        qDebug("Invalid - no '/' in path");    // FIXME
        return;
    }
    desc.fileName = path.mid(idx + 1);

    idx = desc.fileName.lastIndexOf('.');
    if(idx == -1) {
        qDebug("Invalid - dot in filename expected");    // FIXME
    }
    desc.packageType = desc.fileName.mid(idx + 1);
    
    desc.contentType = contentType;
    desc.bInstalled  = bInstalled;
#ifdef DEBUG
    qDebug() << __FUNCTION__ << desc.contentType << desc.bInstalled;
#endif    
    if(m_packages.contains(contentType)) {
        qDebug() << __FUNCTION__ << m_name << " type already added";
        return;
    }

    m_packages[contentType] = desc;
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
    out << m_name << "\t" << m_version 
// FIXME enable when read could parse this
    << "\t" 
    << (isInstalled(BIN) ? "bin:" : ":")
    << (isInstalled(LIB) ? "lib:" : ":")
    << (isInstalled(DOC) ? "doc:" : ":")
    << (isInstalled(SRC) ? "src:" : QString())
    << ";"
    << baseURL << ";"
    << getFileName(BIN) << ";"
    << getFileName(LIB) << ";"
    << getFileName(DOC) << ";"
    << getFileName(SRC)
    << "\n";
    return true;
}

bool Package::read(QTextStream &in)
{
    QString line = in.readLine();
    if (in.atEnd())
        return false;
    QStringList parts = line.split("\t");
    setName(parts.at(0));
    setVersion(parts.at(1));
    QStringList options = parts.at(2).split(";");
    QStringList state = options.at(0).split(":");
    QString baseURL = options.at(1);
    add(baseURL+options.at(2), BIN, state.size() > 0 && state.at(0) == "bin");
    add(baseURL+options.at(3), LIB, state.size() > 0 && state.at(1) == "lib");
    add(baseURL+options.at(4), DOC, state.size() > 0 && state.at(2) == "doc");
    add(baseURL+options.at(5), SRC, state.size() > 0 && state.at(3) == "src");
    return true;
}

void Package::dump(const QString &title)
{
    qDebug() << "class Package dump" << title;
    qDebug() << "m_name:    " << m_name;
    qDebug() << "m_version: " << m_version;
    qDebug() << "m_category: " << m_category;
    qDebug() << "m_deps: " << m_deps.join(" ");

    QHash<Type, PackageItem>::ConstIterator it = m_packages.constBegin();
    for( ; it != m_packages.constEnd(); ++it) {
    	it->dump();
    }
}

bool Package::downloadItem(Downloader *downloader, Package::Type type)
{
    QString URL = getURL(type);
    if (URL.isEmpty())
    {
        qDebug() << __FUNCTION__ << " empty URL for type " << type;
        return false;
    }
    else if(QFile::exists(getFileName(type)))
    {
        qDebug() << __FUNCTION__ << " URL " << URL << " already downloaded for type " << type;
        return false; 
    }
    qDebug() << __FUNCTION__ << " downloading URL " << URL << " for type " << type;
    return downloader->start(URL);
}

bool Package::installItem(Installer *installer, Package::Type type)
{
    QString fileName = getFileName(type);
    if (fileName.isEmpty())
    {
        qDebug() << __FUNCTION__ << " empty fileName for type " << type;
        return false;
    }
    if (!installer->install(fileName)) 
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
