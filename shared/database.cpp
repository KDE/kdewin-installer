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

#include "database.h"
#include "package.h"
#include "packagelist.h"

Database::Database()
        : QObject()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
}

Database::~Database()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    qDeleteAll(m_database);
}


void Database::addPackage(const Package &package)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << package.toString();
#endif

    m_database.append(new Package(package));
}

Package *Database::getPackage(const QString &pkgName, const QByteArray &version)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QList<Package*>::iterator it = m_database.begin();
    for ( ; it != m_database.end(); ++it) {
        if ((*it)->name() == pkgName) {
            if(!version.isEmpty() && (*it)->version() != version)
                continue;
            return (*it);
        }
    }
    return NULL;
}

bool Database::addUnhandledPackages(PackageList *packageList)
{
    QList<Package*>::iterator it = m_database.begin();
    for ( ; it != m_database.end(); ++it) 
    {
        if (!(*it)->handled())
            packageList->addPackage(*(*it));
    }
    return true;
}

void Database::resetHandledState()
{
    QList<Package*>::iterator it = m_database.begin();
    for ( ; it != m_database.end(); ++it) 
    {
        (*it)->setHandled(false);
    }
}

void Database::listPackages(const QString &title)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    qDebug() << title;
    QList<Package*>::iterator it;
    for (it = m_database.begin(); it != m_database.end(); ++it)
        qDebug() << (*it)->toString(true," - ");
}

void Database::queryPackage(const QString &pkgName, bool listFiles)
{
    if (pkgName.isEmpty()) {
        listPackages();
        return;
    }
    Package *pkg = getPackage(pkgName);
    if (!pkg)
        return; 
    QString manifestFile = m_root+"/manifest/"+PackageInfo::manifestFileName(pkg->name(),pkg->version(),Package::BIN);
    QFile file(manifestFile);
    if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QByteArray line = in.readLine().toAscii();
        printf("%s\n",line.data());
    }
}

bool Database::readFromDirectory(const QString &_dir)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QString dir;
    if (_dir.isEmpty())
        dir = m_root + "/manifest"; 
    else
        dir = _dir;
    QDir d(dir);
    d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    d.setNameFilters(QStringList("*.ver"));
    d.setSorting(QDir::Name);

    QFileInfoList list = d.entryInfoList();
    QFileInfo fi;
     
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fi = list[i];        
        QString fileName = fi.fileName();
        QString pkgName;
        QString pkgVersion;
        QString pkgType;
        QString pkgFormat;
        if (!PackageInfo::fromFileName(fileName,pkgName,pkgVersion,pkgType,pkgFormat))
            continue;
        Package *pkg;
        if (pkg = getPackage(pkgName,pkgVersion.toAscii())) 
        {
            Package::PackageItem pi;
            pi.bInstalled =true; 
            pi.setContentType(pkgType);
            pkg->add(pi);
        }
        else 
        {
            Package *pkg =  new Package;
            pkg->setName(pkgName);
            pkg->setVersion(pkgVersion);
            Package::PackageItem pi;
            pi.bInstalled = true;
            pi.setContentType(pkgType);
            pkg->add(pi);
            m_database.append(pkg);
        }
    }        
    emit configLoaded();
    return true;
}

Database &Database::getInstance()
{
    static Database database;
    return database;
}

void Database::dump(const QString &title)
{
    qDebug() << "class Database dump: " << title;
    QList<Package*>::ConstIterator it = m_database.constBegin();
    for ( ; it != m_database.constEnd(); ++it)
        (*it)->dump();
}

#include "database.moc"
