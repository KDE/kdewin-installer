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

Uninstall::Uninstall()
{}

Uninstall::~Uninstall()
{}


bool Uninstall::uninstallPackage(const QString &rootDir, const QString &packageName, bool bUseHashWhenPossible)
{
    QList<FileItem> files;
    QFile f(packageName);

    if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        emit error(QString("Can't open %1 for reading!").arg(packageName));
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
        fileName = rootDir + '/' + fileName.replace("\\ ", " ");
        fi.setFile(fileName);
        if(!fi.exists()) {
            emit warning(QString("File %1 does not exist!").arg(fileName));
            continue;
        }
        files += FileItem(QDir::convertSeparators(fi.absoluteFilePath()), hash);
    }
    f.close();

    QList<FileItem>::ConstIterator it = files.constBegin();
    for( ; it != files.constEnd(); it++ ) {
        FileItem fileItem = *it;
        fi.setFile(fileItem.fileName);

        if(!fi.exists())
            continue;

        if(bUseHashWhenPossible && !fileItem.hash.isEmpty()) {
            // read file & check hash
            f.setFileName(fileName);
            if(!f.open(QIODevice::ReadOnly)) {
                emit warning(QString("Can't open %1 - not removing this file!").arg(fileName));
                continue;
            }
            QByteArray ba = f.readAll();
            f.close();

            if(fileItem.hash != qtMD5(ba)) {
                emit warning(QString("Not removing %1 because hash does not match (locally modified)!").arg(fileName));
                continue;
            }
        }

        if(!QFile::remove(fileItem.fileName)) {
            emit warning(QString("Can't remove %1").arg(fileName));
            continue;
        }
        emit removed(fileItem.fileName);
    }

    return true;
}

#include "uninstall.moc"
