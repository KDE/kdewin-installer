/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker. All rights reserved.
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
#include "hash.h"
#include "hashfile.h"
#include "misc.h"

#include <QFile>
#include <QFileInfo>

HashFile::HashFile(HashFile::Type type, const QString& originalFile) 
    : Hash(type), m_originalFile(originalFile.toUtf8())
{
   
}

HashFile::HashFile(HashFile::Type type, const QByteArray& originalFile) 
    : Hash(type), m_originalFile(originalFile)
{
   
}

HashFile::~HashFile()
{
}

bool HashFile::computeHash()
{
    QFile f(m_originalFile);
    if (!f.open(QIODevice::ReadOnly)) 
    {
        qCritical() << "could not open file" << m_originalFile << " error" << f.error();
        return false;
    }
    m_hash = hash(f);
    if (m_hash.isEmpty())
    {
        qCritical() << "could not compute hash value from " << m_originalFile;
        return false;
    }
    return true;
}

QByteArray HashFile::toHashFileContent()
{
    if (m_hash.isEmpty())
        if (!computeHash())
            return QByteArray();
    
    QFileInfo fi(m_originalFile);
    return m_hash.toHex() + QByteArray("  ") + fi.fileName().toUtf8() + QByteArray("\n");
}

bool HashFile::save(const QString &fileName)
{
    if (m_hash.isEmpty())
        if (!computeHash())
            return false;
    
    QFile hashFile(!fileName.isEmpty() ? fileName : m_originalFile + (m_type == HashFile::Type::MD5 ? ".md5" : ".sha1"));
    if (!hashFile.open(QIODevice::WriteOnly)) 
    {
        qCritical() << "could not create hash file" << hashFile.fileName();
        return false;
    }
    QFileInfo fi(m_originalFile);
    QByteArray hashFileContent = toHashFileContent(); 
    if (hashFileContent.isEmpty())
    {
        qCritical() << "could not create hash file content";
        return false;
    }
    hashFile.write(hashFileContent); 
    hashFile.close();
    return true;
}
