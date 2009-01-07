/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
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

#ifndef HASHFILE_H
#define HASHFILE_H

#include "hash.h"

#include <QString>
#include <QByteArray>

class HashFile : public Hash {
    public: 
        /// create hash instance with the given type and based on the originalFile
        HashFile(Type type, const QString& originalFile);
        HashFile(Type type, const QByteArray& originalFile);
        ~HashFile();

        /**
            compute hash value from original file specified in the constructor  
        */
        bool computeHash();

        /** 
            return hash value as hashfile content 
        */
        QByteArray toHashFileContent();
        
        /**
            save computed hash value into a file. 
            If filename is ommitted use the original filename 
            appended with a hash type extension. 
        */
        bool save(const QString &fileName=QString());
    protected: 
        QByteArray m_hash; 
        QByteArray m_originalFile;
};

#endif
