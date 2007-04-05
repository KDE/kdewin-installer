/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>

#include "uninstall.h"
#include "md5.h"

#include "installerprogress.h"
// FIXME: This should be solved better 
#ifdef USE_GUI
#include "installwizard.h"
extern InstallWizard *wizard;
#endif

Uninstall::Uninstall(const QString &rootDir, const QString &packageName)
: m_rootDir(rootDir), m_packageName(packageName)
{}

Uninstall::~Uninstall()
{}


bool Uninstall::uninstallPackage(bool bUseHashWhenPossible)
{
    QList<FileItem> files;
    QFileInfo fi;
    QFile f;
#ifdef USE_GUI
	InstallerProgress *m_progress = &wizard->installProgressBar();
#else
	InstallerProgress *m_progress = 0;
#endif

	if(!readManifestFile(files))
        return false;

	if (m_progress)
	{
		m_progress->setMaximum(files.size());
		m_progress->show();
	}
    QList<FileItem>::ConstIterator it = files.constBegin();
    for( ; it != files.constEnd(); it++ ) {
        FileItem fileItem = *it;
        fi.setFile(fileItem.fileName);

        if(!fi.exists())
            continue;

        if(bUseHashWhenPossible && !fileItem.hash.isEmpty()) {
            // read file & check hash
            f.setFileName(fileItem.fileName);
            if(!f.open(QIODevice::ReadOnly)) {
                emit warning(QString("Can't open %1 - not removing this file!").arg(fileItem.fileName));
                continue;
            }
            QByteArray ba = f.readAll();
            f.close();

            if(fileItem.hash != qtMD5(ba)) {
                emit warning(QString("Not removing %1 because hash does not match (locally modified)!").arg(fileItem.fileName));
                continue;
            }
        }

        if (m_progress)
        {
            m_progress->setTitle(tr("Removing %1").arg(fileItem.fileName));
        }

		if(!QFile::remove(fileItem.fileName)) {
            emit warning(QString("Can't remove %1").arg(fileItem.fileName));
            continue;
        }
        emit removed(fileItem.fileName);
    }
    if (m_progress)
    {
		m_progress->hide();
	}

    return true;
}

bool Uninstall::checkInstalledFiles()
{
    QList<FileItem> fileList;
    QFileInfo fi;
    QFile f;

    if(!readManifestFile(fileList))
        return false;

    QList<FileItem>::ConstIterator it = fileList.constBegin();
    for( ; it != fileList.constEnd(); it++ ) {
        FileItem fileItem = *it;
        fi.setFile(fileItem.fileName);

        if(!fi.exists())
            emit missing(fileItem.fileName);

        if(!fileItem.hash.isEmpty()) {
            // read file & check hash
            f.setFileName(fileItem.fileName);
            if(!f.open(QIODevice::ReadOnly)) {
                emit warning(QString("Can't open %1 - can't check this file!").arg(fileItem.fileName));
                continue;
            }
            QByteArray ba = f.readAll();
            f.close();

            if(fileItem.hash != qtMD5(ba))
                emit hashWrong(fileItem.fileName);
        }
    }
    return true;
}

bool Uninstall::readManifestFile(QList<FileItem> &fileList)
{
    QFile f(m_packageName);

    if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        emit error(QString("Can't open %1 for reading!").arg(m_packageName));
        return false;
    }
    QByteArray line(1024, 0);
    QString hash, fileName;
    QFileInfo fi;
    int iLength;

    while((iLength = f.readLine(line.data(), line.size()-1)) != -1) {
        QByteArray l = line.left(iLength);
        while(l.endsWith('\n') || l.endsWith('\r'))
            l = l.left(l.length()-1);

        int idx = l.lastIndexOf(' ');
        // not found, or it's an escaped ' '
        if(idx == -1 || (idx > 1 && l[idx-1] == '\\')) {
            hash = QString();
            fileName = QString::fromUtf8(l);
        } else {
             hash = l.mid(idx+1);
             fileName = QString::fromUtf8(l.left(idx));
        }
        fileName = m_rootDir + '/' + fileName.replace("\\ ", " ");
        fi.setFile(fileName);
        if(!fi.exists()) {
            emit warning(QString("File %1 does not exist!").arg(fileName));
            continue;
        }
        fileList += FileItem(QDir::convertSeparators(fi.absoluteFilePath()), hash);
    }
    f.close();

    return true;
}

#include "uninstall.moc"
