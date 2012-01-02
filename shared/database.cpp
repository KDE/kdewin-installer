/****************************************************************************
**
** Copyright (C) 2007-2011 Ralf Habacker. All rights reserved.
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

#include "debug.h"
#include "database.h"
#include "hash.h"
#include "package.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "misc.h"

#include <QDir>
#include <QFileInfo>
#include <QtCore/QTextCodec>

#ifndef DISABLE_SIGNALS
#define DISABLE_SIGNALS
#endif

Database::Database ( QObject *parent )
        : QObject ( parent )
{
// a better way seems to use the following key for a list see http://www.microsoft.com/technet/scriptcenter/csc/scripts/software/installed/cscsw005.mspx
//    HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OggDS

    m_installKeys["vcredist"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\b25099274a207264182f8181add555d0";
// hex would be more natural, implement in the future
//    m_versionKeys["vcredist-800dc10"] = "8.0.50727.762";
    m_versionKeys["vcredist-134274064"] = "8.0.50727.762";

    m_installKeys["psdk-msvc"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\E10DE0A981DFA754BAC98053CDBD71BB";
//    m_versionKeys["psdk-msvc-5020ece"] = "r2";
    m_versionKeys["psdk-msvc-84020942"] = "r2";

    m_installKeys["vcexpress-en-msvc"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\9BA4F6BA58CA200489926BEE5AA53E5A";
//    m_versionKeys["vcexpress-en-msvc-800c628"] = "2005";
    m_versionKeys["vcexpress-en-msvc-134268456"] = "2005";

    m_installKeys["perl"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\0052C8C86573FFC4C8DA8E043AA6BA48";
//    m_versionKeys["perl-5080333"] = "5.8.8";
    m_versionKeys["perl-84411187"] = "5.8.8";

    m_installKeys["tortoisesvn"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\059ABB4F0F655334D839EE46FB5F390A";
//    m_versionKeys["tortoisesvn-10428b9"] = "1.4.3";
    m_versionKeys["tortoisesvn-17049785"] = "1.4.3";
    
#ifndef DISABLE_SIGNALS
    connect ( &Settings::instance(),SIGNAL ( installDirChanged ( const QString & ) ),
              this, SLOT ( slotInstallDirChanged ( const QString & ) ) );
#endif
}

Database::~Database()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    clear();
}

void Database::setRoot( const QString &root ) 
{
    m_root = root;
    reload();
}

void Database::reload() 
{
    clear();
    addFromRegistry();
    readFromDirectory();
}

void Database::addPackage ( const Package &package )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << package.toString();
#endif
    m_database.append ( new Package ( package ) );
}

/// add package if install state is read from registry
void Database::addFromRegistry()
{
#ifdef Q_OS_WIN
    QMap<QString, QString>::ConstIterator i = m_installKeys.begin();
    for ( ;i != m_installKeys.end(); ++i ) {
        if ( !i.key().isEmpty() ) {
            bool ok;
            QString packageCode = getWin32RegistryValue ( hKEY_CLASSES_ROOT, m_installKeys[i.key() ].replace ( "HKEY_CLASSES_ROOT\\","" ), "PackageCode", &ok ).toString();
            if ( !ok ) {
                qDebug() << i.key() << m_installKeys[i.key() ] << "not found.";
                continue;
            }
            qDebug() << i.key() << m_installKeys[i.key() ] << packageCode << "found.";
            QString versionKey = getWin32RegistryValue ( hKEY_CLASSES_ROOT, m_installKeys[i.key() ].replace ( "HKEY_CLASSES_ROOT\\","" ), "Version", &ok ).toString();
            if ( !ok ) {
                qWarning() << i.key() << "version key not found.";
            }
            QString version;
            if (m_versionKeys.contains(i.key() + '-' + versionKey))
                version = m_versionKeys[i.key() + '-' + versionKey];
            else
                qWarning() << "unknown version key" << versionKey << "found";

            Package *pkg =  new Package;
            pkg->setName ( i.key() );
            pkg->setInstalledVersion ( version );
            Package::PackageItem pi( "BIN" );
            pi.setInstalled(true);
            pkg->add ( pi );
            m_database.append ( pkg );
        }
    }
#endif
}


Package *Database::find ( const QString &_name, const QByteArray &version )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    PackageInfo info = PackageInfo::fromString(_name, version);
    QString name = info.name;
    if (!info.compiler.isEmpty())
        name += '-' + info.compiler;
    Q_FOREACH ( Package *pkg, m_database ) {
        if ( pkg->name() == name ) {
            if ( !info.version.isEmpty() && pkg->version() != info.version )
                continue;
            return pkg;
        }
    }
    return NULL;
}

void Database::clear()
{
    qDeleteAll ( m_database );
    m_database.clear();
}

void Database::listPackages ( const QString &title )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    qDebug() << title;
    Q_FOREACH ( const Package *pkg, m_database )
    qDebug() <<  pkg->toString ( true," - " );
}

void Database::listPackageFiles ( const QString &pkgName, FileTypes::Type pkgType )
{
    const QStringList files = getPackageFiles ( pkgName,pkgType );
    qDebug() << files;
}

QStringList Database::getPackageFiles ( const QString &pkgName, FileTypes::Type pkgType )
{
    QStringList files;
    Package *pkg = getPackage ( pkgName );
    if ( !pkg )
        return files;
    QString manifestFile = m_root + "/manifest/" +
                           PackageInfo::manifestFileName ( pkg->name(),pkg->version().toString(),pkgType );
    QFile file ( manifestFile );
    if ( !file.open ( QIODevice::ReadOnly| QIODevice::Text ) )
        return files;

    int iPosFilename = -1;
    QTextStream in ( &file );
    in.setCodec ( QTextCodec::codecForName ( "UTF-8" ) );
    while ( !in.atEnd() ) {
        const QStringList parts = in.readLine().split ( ' ', QString::SkipEmptyParts);
        if ( iPosFilename != -1 && parts.count() > iPosFilename ) {
            files << parts[iPosFilename];
            continue;
        }
        for ( int i = 0; i < parts.count(); i++ ) {
            if ( !Hash::isHash ( parts[i].toUtf8() ) ) {
                iPosFilename = i;
                files << parts[iPosFilename];
                break;
            }
        }
    }
    return files;
}

bool Database::verifyFiles( const QString &pkgName, FileTypes::Type pkgType )
{
    QStringList files;
    Package *pkg = getPackage ( pkgName );
    if ( !pkg )
        return false;
    QString manifestFile = m_root + "/manifest/" +
                           PackageInfo::manifestFileName ( pkg->name(),pkg->version().toString(),pkgType );
    QFile file ( manifestFile );
    if ( !file.open ( QIODevice::ReadOnly| QIODevice::Text ) )
        return false;

    int iPosFilename = -1;
    int iPosHash = 0;
    QTextStream in ( &file );
    in.setCodec ( QTextCodec::codecForName ( "UTF-8" ) );
    while ( !in.atEnd() ) {
        const QStringList parts = in.readLine().split ( ' ', QString::SkipEmptyParts);
        if (iPosFilename == -1) 
        {
            for ( int i = 0; i < parts.count(); i++ ) 
            {
                if ( Hash::isHash ( parts[i].toUtf8() ) ) 
                    iPosHash = i;
                else
                    iPosFilename = i;
            }
        }
        if ( iPosFilename >= 0 && iPosHash >= 0) 
        {
            QByteArray checkSum = Hash::instance().hash(parts[iPosFilename]).toHex();
            if (parts[iPosHash] != checkSum)
                printf("%s checksum failed\n",qPrintable(parts[iPosFilename]));
        }
    }
    return true;
}

bool Database::readFromDirectory ( const QString &_dir )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString dir = ( _dir.isEmpty() ) ? m_root + "/manifest" : _dir;
    QDir d ( dir );
    d.setFilter ( QDir::NoDotAndDotDot | QDir::AllEntries );
    d.setNameFilters ( QStringList ( "*.ver" ) );
    d.setSorting ( QDir::Name );

    Q_FOREACH ( const QFileInfo &fi, d.entryInfoList() ) {
        QString fileName = fi.fileName();
        QString pkgName;
        QString pkgVersion;
        QString pkgType;
        QString pkgFormat;
        if ( !PackageInfo::fromFileName ( fileName,pkgName,pkgVersion,pkgType,pkgFormat ) )
            continue;
        Package *pkg;
        if ( ( pkg = getPackage ( pkgName,pkgVersion.toAscii() ) ) != NULL ) {
            Package::PackageItem pi ( pkgType );
            pi.setInstalled(true);
            pkg->add ( pi );
            qDebug() << "set package installed state" << pi << "for package" << *pkg;
        } else {
            Package *pkg =  new Package;
            pkg->setName ( pkgName );
            pkg->setInstalledVersion ( pkgVersion );
            Package::PackageItem pi ( pkgType );
            pi.setInstalled(true);
            pkg->add ( pi );
            m_database.append ( pkg );
            qDebug() << "added" << *pkg;
        }
    }
    emit configLoaded();
    return true;
}

bool Database::isAnyPackageInstalled ( const QString &installRoot )
{
    return QDir ( installRoot+ "/manifest" ).exists();
}

bool Database::isAnyPackageInstalled()
{
    if (m_root.isEmpty())
    {
        qCritical() << "no install root defined";
        return false;
    }
    return QDir ( m_root + "/manifest" ).exists();
}

void Database::slotInstallDirChanged ( const QString &dir )
{
    clear();
    readFromDirectory ( dir );
    addFromRegistry();
}

QDebug & operator<< ( QDebug &out, const Database &c )
{
    out << "Database ("
    << "m_root:" << c.m_root
    << "QList<Package*> (size:" << c.m_database.size();

    Q_FOREACH ( const Package *pkg, c.m_database ) {
        out << *pkg;
    }

    out << ") )";
    return out;
}
