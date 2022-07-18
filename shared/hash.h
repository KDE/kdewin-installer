/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef HASH_H
#define HASH_H

class QFile;
class QCryptographicHash;

#include <QByteArray>
#include <QString>

class Hash {
    public:
        typedef enum { None, MD5, SHA1 } Type; 
        
        Hash(Type type = MD5);
        ~Hash();
        
        /**
            set hash type 
            note that setting the type after already 
            calling addData() will reset the internal hash value
        */
        void setType(Type type);
        bool setType(const QString &type);

        /// return hash type
        Hash::Type type() { return m_type; }

        /// compute hash from file content 
        QByteArray hash(QFile &f);

        /// compute hash from file content 
        QByteArray hash(const QString &file);

        /// compute hash from byte array
        QByteArray hash(QByteArray &data);

        /// the following three methods supports adding partial data 
        /// reset hash value
        void reset();
        /// Adds the first length chars of data to the cryptographic hash.
        void addData( const char *data, int size );
        /// Returns the final hash value.
        QByteArray result() const;
        
        /// validate hash string
        static bool isHash (const QByteArray &str);

        /// return type of hash
        static Hash::Type isType(const QByteArray &str);

        static Hash &instance();
        
        /// return md5 hash 
        static QByteArray md5(QFile &f);
        static QByteArray md5(const QString &f);
        /// return sha1 hash 
        static QByteArray sha1(QFile &f);
        static QByteArray sha1(const QString &f);
    protected: 
        Type m_type;
        QCryptographicHash *m_cryptoHash;
};

/**
 holds a hash value and type
*/ 
class HashValue
{
public:
    HashValue(Hash::Type type = Hash::MD5, QByteArray value=QByteArray())
        : m_type(type), m_value(value)
    {
    }
    
    Hash::Type type() { return m_type; }
    QString typeAsString() { return m_type == Hash::MD5 ? QLatin1String("md5") : QLatin1String("sha1"); }
    QByteArray value() { return m_value; }

private:
    Hash::Type m_type;
    QByteArray m_value;
};

#endif
