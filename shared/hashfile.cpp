/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "config.h"
#include "hash.h"
#include "hashfile.h"
#include "misc.h"

#include <QByteArray>
#include <QList>
#include <QFile>
#include <QFileInfo>

HashFile::HashFile(HashFile::Type type, const QString &fileName, const QString &basePath) 
    : Hash(type),
      m_installFile(InstallFile(fileName)),
      m_fullName(basePath.isEmpty() ? fileName : basePath + '/' + fileName)
{
   
}
HashFile::HashFile(HashFile::Type type, const InstallFile &file, const QString &basePath)
    : Hash(type),
      m_installFile(file),
      m_fullName( (basePath.isEmpty() || m_installFile.bAbsInputPath) ? file.inputFile : basePath + '/' + file.inputFile)
{

}

HashFile::~HashFile()
{
}

bool HashFile::computeHash()
{
    QFile f(m_fullName);
    if (!f.open(QIODevice::ReadOnly)) 
    {
        qCritical() << "could not open file" << m_fullName << " error" << f.error();
        return false;
    }
    m_hash = hash(f);
    if (m_hash.isEmpty())
    {
        qCritical() << "could not compute hash value from " << m_fullName;
        return false;
    }
    return true;
}

QByteArray HashFile::toHashFileContent()
{
    if (m_hash.isEmpty())
        if (!computeHash())
            return QByteArray();

    QString fileName = m_installFile.outputFile.isEmpty() ? m_installFile.inputFile : m_installFile.outputFile;
    fileName.replace(' ', "\\ "); // escape
    return m_hash.toHex() + QByteArray("  ") + fileName.toUtf8() + QByteArray("\n");
}

bool HashFile::save(const QString &fileName)
{
    if (m_hash.isEmpty())
        if (!computeHash())
            return false;
    
    QFile f(!fileName.isEmpty() ? fileName : m_fullName + fileNameExtension());
    if (!f.open(QIODevice::WriteOnly)) 
    {
        qCritical() << "could not create hash file" << f.fileName();
        return false;
    }
    QFileInfo fi(m_fullName);
    const QByteArray hashFileContent = toHashFileContent(); 
    if (hashFileContent.isEmpty())
    {
        qCritical() << "could not create hash file content";
        return false;
    }
    f.write(hashFileContent); 
    f.close();
    return true;
}

bool HashFile::isHashFileName(const QString &fileName)
{
    return fileName.endsWith(QLatin1String(".md5")) || fileName.endsWith(QLatin1String(".sha1"));
}

bool HashFile::readFromFile(const QString &fileName)
{
    QFile f(!fileName.isEmpty() ? fileName : m_fullName + fileNameExtension());
    if (!f.open(QIODevice::ReadOnly)) 
        return false;
    const QList<QByteArray> parts = f.readLine().trimmed().split(' ');
    if (parts.size() != 3)
    {
        qCritical() << "invalid hash file format" << f.fileName();
        return false;
    }
    if (!isHash(parts[0]))
        return false;

    m_hash = parts[0];
    return true;
}

QString HashFile::fileNameExtension() const
{
    if (m_type == HashFile::MD5)
        return ".md5";
    else if (m_type == HashFile::SHA1)
        return ".sha1";
    else 
        return ".none";
}

QByteArray HashFile::getHash() const
{   
    return m_hash;
}
