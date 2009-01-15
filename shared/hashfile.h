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
        typedef Hash::Type Type; 
        /// create hash instance with the given type and based on the originalFile
        HashFile(Type type, const QString &fileName, const QString &basePath = QString());
        ~HashFile();

        /**
            compute hash value from original file specified in the constructor  
        */
        bool computeHash();

        /**
            return internal stored hash value
        */
        QByteArray getHash() const;

        /** 
            return hash value as hashfile content 
        */
        QByteArray toHashFileContent();
        
        /**
            read hash file from fileName and return 
            true if the hash value could be read successfull. 
            If fileName is ommitted the hash file name is 
            build from the original file appended with an 
            extension depending on the hash type. 
        */
        bool readFromFile(const QString &fileName=QString());
        /**
            save computed hash value into a file. 
            If filename is ommitted use the original filename 
            appended with an extension depending on the hash type. 
        */
        bool save(const QString &fileName=QString());
        
        /** 
            return hash file extension for the currently used type
        */
        QString fileNameExtension() const;
        
        /** 
            return true if the given filename is a valid name for 
            a hash file
        */
        static bool isHashFileName(const QString &fileName);
    protected: 
        QByteArray m_hash; 
        QString m_fileName;
        QString m_fullName;
};

#endif
