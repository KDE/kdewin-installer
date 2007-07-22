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

#ifndef UNINSTALL_H
#define UNINSTALL_H

#include <QObject>

/* Uninstall class to uninstall a complete package defined by a manifest file */
class Uninstall : public QObject
{
    Q_OBJECT
protected:
    struct FileItem {
        QString fileName;
        QString hash;  

        FileItem(const QString &fn, const QString &h) { fileName = fn; hash = h; }
    };
    QString m_rootDir;
    QString m_packageName;

public:
    Uninstall(const QString &rootDir, const QString &packageName);
    ~Uninstall();

    /*
      uninstalls all files from this package
      makes a modified check with checksum if wanted
    */
    bool uninstallPackage(bool bUseHashWhenPossible=true);
    /*
      checks if all files of the installed package are still available
      if no hash is provided in manifest, only check if exists is performed, 
      otherwise also the checksum is compared
    */
    bool checkInstalledFiles();

    /** 
      delete directories containing no files
    */ 
    bool cleanDirectories();
    
protected:
    bool readManifestFile(QList<FileItem> &fileList);
Q_SIGNALS:
    void missing(const QString &fileName);  // a file which should be installed, is missing
    void hashWrong(const QString &fileName);// a file which is installed has a wrong checksum -> locally modified
    void removed(const QString &fileName);  // a file was successfully removed
    void warning(const QString &wrnMsg);    // a warning message
    void error(const QString &errMsg);      // an error message
};

#endif  // UNINSTALL_H
