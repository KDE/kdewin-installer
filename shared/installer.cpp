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

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "config.h"
#include "installer.h"
#include "installerprogress.h"
#include "packagelist.h"
#include "unpacker.h"

//#define DEBUG
#ifdef Q_CC_MSVC
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

Installer::Installer(InstallerProgress *_progress)
        : QObject(), m_progress(_progress), m_type(Installer::Standard)
{
    m_root = ".";
//  packageList = _packageList;
//  if(packageList)
//  {
//      packageList->m_root = m_root;
//  }
//  connect (packageList,SIGNAL(configLoaded()),this,SLOT(updatePackageList()));
}

Installer::~Installer()
{}

void Installer::setRoot(const QString &root)
{
    m_root = root;
//    packageList->m_root = m_root = root;
//    QDir().mkdir(m_root);
}

bool Installer::isEnabled()
{
    return true;
}

bool Installer::loadConfig()
{
#ifdef DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
    return true;
}

void Installer::updatePackageList()
{
#ifdef DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif

    loadConfig();
}

bool Installer::createManifestFile()
{
    QString destpath = m_root;
    QString mFile = m_packageToInstall->manifestFileName(m_installType);
    QString vFile = m_packageToInstall->versionFileName(m_installType);

    QFileInfo a(destpath +"/manifest/"+ mFile);

    // write manifest file by ourself if it is not exist or is corrupted 
    // or path relocations had been applied
    if (m_packageToInstall->pathRelocations().size() == 0 && 
            a.exists() && a.isFile() && a.size() > 0)
        return true;
    
    
    // in some gnuwin32 packages the manifest file is a directory
    if (a.isDir())
    {
        QDir().rmdir(a.absoluteFilePath());
    }
    m_files << "manifest/"+ mFile;
    m_files << "manifest/"+ vFile;
    QFile mf(a.absoluteFilePath());
    if (!mf.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream mo(&mf);
    mo.setCodec(QTextCodec::codecForName("UTF-8"));
    mo << m_files.join("\n");
    mf.close();

    // write .ver file if not exist
    QFileInfo b(destpath +"/manifest/"+ vFile);
    if (b.exists() && b.isFile() && b.size() > 0)
        return true;
    QFile vf(b.absoluteFilePath());
    if (!vf.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        mf.remove();
        return false;
    }
    QTextStream vo(&vf);
    vo.setCodec(QTextCodec::codecForName("UTF-8"));
    vo << m_packageToInstall->name() + " " + m_packageToInstall->version() + " " + m_packageToInstall->notes() + "\n";
    vo << m_packageToInstall->name() + ":\n";
    vf.close();

    return true;
}

bool Installer::createManifestFileForExecutable()
{
    QString mFile = m_packageToInstall->manifestFileName(Package::BIN);
    QString vFile = m_packageToInstall->versionFileName(Package::BIN);
    QFile mf(m_root + "/manifest/" + mFile);
    if (!mf.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream mo(&mf);
    mo << "manifest/"+ mFile + "\n";
    mo << "manifest/"+ vFile + "\n";
    mf.close();

    QFile vf(m_root + "/manifest/" + vFile);
    if (!vf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mf.remove();
        return false;
    }
    QTextStream vo(&vf);
    vo << m_packageToInstall->name() + " " + m_packageToInstall->version() + " " + m_packageToInstall->notes() + "\n";
    vo << m_packageToInstall->name() + ":\n";
    vf.close();
    return true;
}

bool Installer::createQtConfigFile()
{
    if(!QFile::exists(m_root + "/bin/qt.conf")) {
        QSettings qtConfig(m_root + "/bin/qt.conf",QSettings::IniFormat);
        qtConfig.setValue("Paths/Prefix",       "");
        qtConfig.setValue("Paths/Documentation","../doc");
        qtConfig.setValue("Paths/Headers",      "../include");
        qtConfig.setValue("Paths/Libraries",    "../lib");
        qtConfig.setValue("Paths/Plugins",      "../plugins");
        qtConfig.setValue("Paths/Translations", "../translations");
        qtConfig.setValue("Paths/Settings",     "../etc");
        qtConfig.setValue("Paths/Examples",     "../examples");
        qtConfig.setValue("Paths/Demos",        "../demos");
        qtConfig.sync();
    }
    return true;
}

bool Installer::install(Package *pkg, const Package::Type type, const QString &fileName, const StringHash &pathRelocations)
{
    m_packageToInstall = pkg;
    m_installType = type;

    Unpacker p(m_progress, this);
    if(!p.unpackFile(fileName, m_root, pathRelocations))
      return false;

    m_files = p.getUnpackedFiles();
    createManifestFile();

    QFileInfo fi(fileName);
    if(fi.fileName().startsWith("qt"))
        createQtConfigFile();

    return true;
}

#include "installer.moc"
