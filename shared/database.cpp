/****************************************************************************
**
** Copyright (C) 2007 Ralf Habacker. All rights reserved.
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

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QtCore/QTextCodec>
#include "database.h"
#include "package.h"
#include "packagelist.h"
#include "misc.h"

Database::Database ( QObject *parent )
        : QObject ( parent )
{
    m_installKeys["vcredist"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\b25099274a207264182f8181add555d0";
    m_installKeys["psdk-msvc"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\E10DE0A981DFA754BAC98053CDBD71BB";
    m_installKeys["vcexpress-en-msvc"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\9BA4F6BA58CA200489926BEE5AA53E5A";
    m_installKeys["perl"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\0052C8C86573FFC4C8DA8E043AA6BA48";
    m_installKeys["tortoisesvn"] = "HKEY_CLASSES_ROOT\\Installer\\Products\\059ABB4F0F655334D839EE46FB5F390A";
    addFromRegistry();
    connect ( &Settings::instance(),SIGNAL ( installDirChanged ( const QString & ) ),
              this, SLOT ( slotInstallDirChanged ( const QString & ) ) );
}

Database::~Database()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    clear();
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
            Package *pkg =  new Package;
            pkg->setName ( i.key() );
            pkg->setInstalledVersion ( "" );
            Package::PackageItem pi;
            pi.bInstalled = true;
            pi.setContentType ( "BIN" );
            pkg->add ( pi );
            m_database.append ( pkg );
        }
    }
#endif
}


Package *Database::getPackage ( const QString &pkgName, const QByteArray &version )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    Q_FOREACH ( Package *pkg, m_database ) {
        if ( pkg->name() == pkgName ) {
            if ( !version.isEmpty() && pkg->version() != version )
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

void Database::listPackageFiles ( const QString &pkgName, Package::Type pkgType )
{
    QStringList files = getPackageFiles ( pkgName,pkgType );
    qDebug() << files;
}

extern bool isHash ( const QByteArray &str );
QStringList Database::getPackageFiles ( const QString &pkgName, Package::Type pkgType )
{
    QStringList files;
    Package *pkg = getPackage ( pkgName );
    if ( !pkg )
        return files;
    QString manifestFile = m_root + "/manifest/" +
                           PackageInfo::manifestFileName ( pkg->name(),pkg->version(),pkgType );
    QFile file ( manifestFile );
    if ( !file.open ( QIODevice::ReadOnly| QIODevice::Text ) )
        return files;

    int iPosFilename = -1;
    QTextStream in ( &file );
    in.setCodec ( QTextCodec::codecForName ( "UTF-8" ) );
    while ( !in.atEnd() ) {
        QStringList parts = in.readLine().split ( ' ' );
        if ( iPosFilename != -1 && parts.count() > iPosFilename ) {
            files << parts[iPosFilename];
            continue;
        }
        for ( int i = 0; i < parts.count(); i++ ) {
            if ( !isHash ( parts[i].toUtf8() ) ) {
                iPosFilename = i;
                files << parts[iPosFilename];
                break;
            }
        }
    }
    return files;
}

bool Database::readFromDirectory ( const QString &_dir )
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString dir = ( _dir.isEmpty() ) ? m_root + "/manifest" : dir;
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
            Package::PackageItem pi;
            pi.bInstalled = true;
            pi.setContentType ( pkgType );
            pkg->add ( pi );
        } else {
            Package *pkg =  new Package;
            pkg->setName ( pkgName );
            pkg->setInstalledVersion ( pkgVersion );
            Package::PackageItem pi;
            pi.bInstalled = true;
            pi.setContentType ( pkgType );
            pkg->add ( pi );
            m_database.append ( pkg );
        }
    }
    emit configLoaded();
    return true;
}

bool Database::isAnyPackageInstalled ( const QString &installRoot )
{
    return QDir ( installRoot+ "/manifest" ).exists();
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

#include "database.moc"
