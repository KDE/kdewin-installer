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
#include "debug.h"
#include "externalinstallercontrol.h"
#include "installer.h"
#include "installerprogress.h"
#include "packagelist.h"
#include "unpacker.h"
#include "uninstaller.h"
#include "typehelper.h"

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

//#define DEBUG
#ifdef Q_CC_MSVC
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

Installer::Installer(QObject *parent)
        : QObject(parent), m_type(Installer::Standard)
{
    m_root = ".";
}

Installer::~Installer()
{
}

void Installer::setRoot(const QString &root)
{
    m_root = root;
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
    /// @TODO: add md5 hashes 
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
    vo << m_packageToInstall->name() + " " + m_packageToInstall->version().toString() + " " + m_packageToInstall->notes() + "\n";
    vo << m_packageToInstall->name() + ":\n";
    vf.close();

    return true;
}

bool Installer::createManifestFileForExecutable()
{
    QString mFile = m_packageToInstall->manifestFileName(FileTypes::BIN);
    QString vFile = m_packageToInstall->versionFileName(FileTypes::BIN);
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
    vo << m_packageToInstall->name() + " " + m_packageToInstall->version().toString() + " " + m_packageToInstall->notes() + "\n";
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

bool Installer::install(Package *pkg, const FileTypes::FileType type)
{
    m_packageToInstall = pkg;
    m_installType = type;
    QString fileName = pkg->localFileName(type); 
    qDebug() << __FUNCTION__ << "filename: " << fileName << "type: " << type;

    if (fileName.endsWith(".exe")) 
    {
        if (!installExecutable(pkg,type))
            return false;
        // mark install state  for accessing deinstaller on  remove    
        m_files = QStringList() << pkg->localFilePath(type);
    }
    else if (fileName.endsWith(".msi")) 
    {
        if (!installMsiPackage(pkg,type))
            return false;
        // mark install state  for accessing deinstaller on  remove    
        m_files = QStringList() << pkg->localFilePath(type);
    }
    else {
        connect(Unpacker::instance(),SIGNAL(error(const QString &)),this,SLOT(slotError(const QString &)));
        bool ret = Unpacker::instance()->unpackFile(pkg->localFilePath(type), m_root, pkg->pathRelocations());
        disconnect(Unpacker::instance(),SIGNAL(error(const QString &)),this,SLOT(slotError(const QString &)));
        if (!ret)
            return false;
        m_files = Unpacker::instance()->unpackedFiles();
    }

    qSort(m_files);
    createManifestFile();
    QString postInstall = QString("manifest/post-install-%1.cmd").arg(fileName);
    if(m_files.contains(postInstall))
      handlePostInstall(root() + '/' + postInstall);

    QFileInfo fi(fileName);
    if(fi.fileName().startsWith("qt"))
        createQtConfigFile();

    return true;
}

bool Installer::installExecutable(Package *pkg, FileTypes::FileType type)
{
    QString fileName = pkg->localFilePath(type);
#ifdef Q_OS_WIN
    QProcess proc;
    proc.setWorkingDirectory(QDir::tempPath());
    QStringList params( "/Q" );
    if (fileName.contains("vcredist"))
        params << "/p";
    proc.start (fileName, params);   // FIXME: don't hardcode command line parameters!
    if ( !proc.waitForStarted() )
        return false;

    InstallerControlTypeList *l = (InstallerControlTypeList *)pkg->userData(0);
    if (l)
    {
        ExternalInstallerControl e;
        e.connect(proc);
        Q_FOREACH(InstallerControlType item, *l)
        {
            if (item.type() == "Button")
                e.pressButton(item.caption());
        }
    }
    do {
        Sleep(50);
    } while ( !proc.waitForFinished() );

    return ( proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0 );
#else
    emit error ( tr ( "Don't know how to execute %1 on a non windows system." ).arg ( fileName ) );
    return false;
#endif
}

bool Installer::installMsiPackage(Package *pkg, FileTypes::FileType type)
{
    QString fileName = pkg->localFilePath(type);
#ifdef Q_OS_WIN
    QProcess proc;
    proc.start ( "msiexec", QStringList() << "/I" << QDir::toNativeSeparators ( fileName ) );
    if ( !proc.waitForStarted() )
        return false;
    do {
        Sleep (50);
    } while (!proc.waitForFinished());

    return (proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0);
#else
    emit error ( tr ( "Don't know how to execute %1 on a non windows system." ).arg ( fileName ) );
    return false;
#endif
}

bool Installer::handlePostInstall(const QString &postInstall)
{
    qDebug() << __FUNCTION__ << "postInstall: " << postInstall;

    QStringList args;
    args << "/C" << postInstall;
    QProcess p(this);
    p.setWorkingDirectory(m_root);
    p.start("cmd.exe", args);
    if(!p.waitForStarted())
        return false;
    do {
        qApp->processEvents();
    } while(!p.waitForFinished(100));
    return true;
}

bool Installer::uninstall(const QString &pathToManifest)
{
    qDebug() << __FUNCTION__ << "pathToManifest: " << pathToManifest;
    return Uninstaller::instance()->uninstallPackage(pathToManifest, m_root);
}

void Installer::cancel()
{
    Unpacker::instance()->cancel();
    Uninstaller::instance()->cancel();
}

void Installer::slotError(const QString &msg)
{
    emit error(msg);
}

InstallerProgress *Installer::progress()
{
    return Unpacker::instance()->progress();
}
