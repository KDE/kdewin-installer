/****************************************************************************
**
** Copyright (C) 2005-2006 Ralf Habacker. All rights reserved.
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

#include <QtCore>
#include <QtDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


#include "installer.h"
#include "packagelist.h"
#include "installerprogress.h"

#include "quazip.h"
#include "quazipfile.h"
#include "qua7zip.h"

using namespace qua7zip;

//#define DEBUG
#ifdef Q_CC_MSVC
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

Installer::Installer(PackageList *_packageList, InstallerProgress *_progress)
        : QObject(), m_progress(_progress), m_type(Installer::Standard)
{
    m_root = ".";
    packageList = _packageList;
    if(packageList)
    {
        packageList->m_installer = this;
        packageList->m_root = m_root;
    }
    connect (packageList,SIGNAL(loadedConfig()),this,SLOT(updatePackageList()));
}

Installer::~Installer()
{}

void Installer::setRoot(const QString &root)
{
    packageList->m_root = m_root = root;
    QDir().mkdir(m_root);
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
       // this belongs to PackageList
    if (m_type == GNUWIN32)
    {
        // gnuwin32 related
        QDir dir(m_root + "/manifest");
        dir.setFilter(QDir::Files);
        dir.setNameFilters(QStringList("*.ver"));
        dir.setSorting(QDir::Size | QDir::Reversed);

        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list[i];
            Package pkg;
            pkg.setFromVersionFile(fileInfo.fileName());
            packageList->setInstalledPackage(pkg);
        }
    }
    else
    {
        // default installer
        // update config from package file
    }
    return true;
}

void Installer::updatePackageList()
{
#ifdef DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif

    loadConfig();
}

#ifndef QUNZIP_BUFFER
# define QUNZIP_BUFFER (256 * 1024)
#endif
bool Installer::unzipFile(const QString &destpath, const QString &zipFile, const StringHash &pathRelocations)
{
    QDir path(destpath);
    QuaZip z(zipFile);

    if(!z.open(QuaZip::mdUnzip))
    {
        setError(tr("Can not open %1").arg(zipFile));
        return false;
    }

    if(!path.exists())
    {
        setError(tr("Internal Error - Path %1 does not exist").arg(path.absolutePath()));
        return false;
    }

    // z.setFileNameCodec("Windows-1252"); // important!

    QuaZipFile file(&z);
    QuaZipFileInfo info;
    if (m_progress)
    {
        m_progress->setMaximum(z.getEntriesCount());
        m_progress->show();
    }

    for(bool bOk = z.goToFirstFile(); bOk; bOk = z.goToNextFile())
    {
        // get file information
        if(!z.getCurrentFileInfo(&info))
        {
            setError(tr("Can not get file information from zip file %1").arg(zipFile));
            return false;
        }
        // relocate path names
        QString outPath = path.filePath(info.name);
        for(StringHash::const_iterator i = pathRelocations.constBegin(); i != pathRelocations.constEnd(); i++)
        {
            outPath.replace(i.key(),i.value());
        }
        QFileInfo fi(outPath);

        // is it's a subdir ?
        if(info.compressedSize == 0 && info.uncompressedSize == 0)
        {
            if(fi.exists())
            {
                if(!fi.isDir())
                {
                    setError(tr("Can not create directory %1").arg(fi.absoluteFilePath()));
                    return false;
                }
                continue;
            }
            if(!path.mkpath(fi.absoluteFilePath()))
            {
                setError(tr("Can not create directory %1").arg(fi.absolutePath()));
                return false;
            }
            continue;
        }
        // some archives does not have directory entries
        else
        {
            if(!path.exists(fi.absolutePath()))
            {
                if (!path.mkpath(fi.absolutePath()))
                {
                    setError(tr("Can not create directory %1").arg(fi.absolutePath()));
                    return false;
                }
            }
        }
        // open file
        if(!file.open(QIODevice::ReadOnly))
        {
            setError(tr("Can not open file %1 from zip file %2").arg(info.name).arg(zipFile));
            return false;
        }
        if(file.getZipError() != UNZ_OK)
        {
            setError(tr("Error reading zip file %1").arg(zipFile));
            return false;
        }

        // create new file
        QFile newFile(fi.absoluteFilePath());
        if(!newFile.open(QIODevice::WriteOnly))
        {
            setError(tr("Can not creating file %1").arg(fi.absoluteFilePath()));
            return false;
        }

        if (m_progress)
        {
            m_progress->setTitle(tr("Installing %1").arg(newFile.fileName()));
        }
        // copy data
        // FIXME: check for not that huge filesize ?
        qint64 iBytesRead;
        QByteArray ba;
        ba.resize(QUNZIP_BUFFER);

        while((iBytesRead = file.read(ba.data(), QUNZIP_BUFFER)) > 0)
            newFile.write(ba.data(), iBytesRead);

        file.close();
        newFile.close();

        if(file.getZipError() != UNZ_OK)
        {
            setError(tr("Error reading zip file %1").arg(zipFile));
            return false;
        }
    }
    z.close();
    if(z.getZipError() != UNZ_OK)
    {
        setError(tr("Error reading zip file %1").arg(zipFile));
        return false;
    }
    return true;
}

bool Installer::un7zipFile(const QString &destpath, const QString &zipFile)
{
    QDir path(destpath);
    Qua7zip z(zipFile);

    if(!z.open(Qua7zip::mdUnpack))
    {
        setError(tr("Can not open %1").arg(zipFile));
        return false;
    }

    if(!path.exists())
    {
        setError(tr("Internal Error - Path %1 does not exist").arg(path.absolutePath()));
        return false;
    }

    // z.setFileNameCodec("Windows-1252"); // important!

    Qua7zipFile file(&z);
    Qua7zipFileInfo info;
    if (m_progress)
    {
        m_progress->setMaximum(z.getEntriesCount());
        m_progress->show();
    }

    for(bool bOk = z.goToFirstFile(); bOk; bOk = z.goToNextFile())
    {
        // get file information
        if(!z.getCurrentFileInfo(info))
        {
            setError(tr("Can not get file information from zip file %1").arg(zipFile));
            return false;
        }
        QFileInfo fi(path.filePath(info.fileName));

        // is it's a subdir ?
        if(info.isDir)
        {
            if(fi.exists())
            {
                if(!fi.isDir())
                {
                    setError(tr("Can not create directory %1").arg(fi.absoluteFilePath()));
                    return false;
                }
                continue;
            }
            if(!path.mkpath(fi.absoluteFilePath()))
            {
                setError(tr("Can not create directory %1").arg(fi.absolutePath()));
                return false;
            }
            continue;
        }
        // some archives does not have directory entries
        else
        {
            if(!path.exists(fi.absolutePath()))
            {
                if (!path.mkpath(fi.absolutePath()))
                {
                    setError(tr("Can not create directory %1").arg(fi.absolutePath()));
                    return false;
                }
            }
        }
        // open file
        if(!file.open(QIODevice::ReadOnly))
        {
            setError(tr("Can not open file %1 from zip file %2").arg(info.fileName).arg(zipFile));
            return false;
        }
        if(file.get7zipError() != SZ_OK)
        {
            setError(tr("Error reading zip file %1").arg(zipFile));
            return false;
        }

        // create new file
        QFile newFile(fi.absoluteFilePath());
        if(!newFile.open(QIODevice::WriteOnly))
        {
            setError(tr("Can not creating file %1").arg(fi.absoluteFilePath()));
            return false;
        }

        if (m_progress)
        {
            m_progress->setTitle(tr("Installing %1").arg(newFile.fileName()));
        }
        // copy data
        // FIXME: check for not that huge filesize ?
        qint64 iBytesRead;
        QByteArray ba;
        ba.resize(QUNZIP_BUFFER);

        while((iBytesRead = file.read(ba.data(), QUNZIP_BUFFER)) > 0)
            newFile.write(ba.data(), iBytesRead);

        file.close();
        newFile.close();

        if(file.get7zipError() != SZ_OK)
        {
            setError(tr("Error reading zip file %1").arg(zipFile));
            return false;
        }
    }
    z.close();
    if(z.get7zipError() != SZ_OK)
    {
        setError(tr("Error reading zip file %1").arg(zipFile));
        return false;
    }
    return true;
}


void Installer::setError(const QString &str)
{
    // FIXME: merge with Downloader::setError
    qDebug(str.toLocal8Bit().data());
    QFile f("kdewin-installer.log");
    if(f.open(QIODevice::WriteOnly)) {
        f.write(QDateTime::currentDateTime().toString("yymmdd,hh:mm: ").toLocal8Bit().data());
        f.write(str.toLocal8Bit().data());
        f.write("\n");
        f.close();
    }
}

bool Installer::createQtConfigFile()
{
    QSettings qtConfig(m_root + "/bin/qt.conf",QSettings::IniFormat);
    qtConfig.setValue("Paths/Prefix",m_root);
    qtConfig.setValue("Translations","i18n");
    qtConfig.sync();
    return true;
}

bool Installer::install(const QString &fileName, const StringHash &pathRelocations)
{
    if (fileName.endsWith(".zip"))
    {
        int ret = unzipFile(m_root, fileName,pathRelocations);
        if (ret && fileName.startsWith("qt"))
            createQtConfigFile();
        return ret; 
    }
    if (fileName.endsWith(".7z"))
    {
        return un7zipFile(m_root, fileName);
    }
#ifdef Q_WS_WIN
    else // for all other formats use windows assignments
    {
        // fixme: use QProcess to determine if all worked fine?
        ShellExecuteW(0, L"open", (WCHAR*)fileName.utf16(), NULL, NULL, SW_SHOWNORMAL);
        return true;
    }
#else
#endif
    return false;
}

#include "installer.moc"
