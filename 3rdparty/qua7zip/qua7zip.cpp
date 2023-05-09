/*
 * cpp7zip.cpp - unpack 7zip files
 * Copyright (C) 2007  Igor Mironchick <imironchick@gmail.com>
 * Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

/*!	\file	cppSzip.cpp
	\brief	C++ interface for Szip.

    The interface is derived from quazip (quazip.sf.net)
    - special thx to Sergey A. Tachenov
*/

#include "qua7zip.h"

namespace qua7zip {

/*
    class Qua7zipFile
*/
Qua7zipFile::Qua7zipFile(Qua7zip *manager, QObject *parent)
    :   QIODevice(parent)
    ,   m_manager(manager)
    ,   res( SZ_OK )
{
}

Qua7zipFile::~Qua7zipFile()
{
    if(isOpen())
        close();
}

QString Qua7zipFile::get7zipName() const
{
    return (m_manager ? m_manager->get7zipName() : QString());
}

QString Qua7zipFile::getFileName() const
{
  if(m_manager == NULL || (openMode()&WriteOnly))
      return QString();
  QString name = m_manager->getCurrentFileName();
  if(name.isNull())
      return QString(); // FIXME
//    setZipError(zip->getZipError());
  return name;
}

bool Qua7zipFile::getFileInfo(Qua7zipFileInfo &info) const
{
    if(m_manager == NULL || m_manager->getMode() != Qua7zip::mdUnpack)
      return false;
    return m_manager->getCurrentFileInfo(info);
}

bool Qua7zipFile::open(OpenMode mode)
{
    if(mode != QIODevice::ReadOnly) {
        qWarning("Qua7zip::open(): only 7zip read support");
        return false;
    }
    if(isOpen()) {
        qWarning("Qua7zip::open(): already opened");
        return false;
    }
    if(mode&Unbuffered) {
        qWarning("Qua7zip::open(): Unbuffered mode is not supported");
        return false;
    }
    setOpenMode(mode);
    m_iCurReadOfs = 0;
    m_iUnpackedSize = 0;
    m_iStartReadOfs = 0;

    return true;
}

void Qua7zipFile::close()
{
    m_iCurReadOfs = 0;
    m_iUnpackedSize = 0;
    m_iStartReadOfs = 0;
    setOpenMode(QIODevice::NotOpen);
}

bool Qua7zipFile::isSequential() const
{
    return true;
}

qint64 Qua7zipFile::readData(char *data, qint64 maxSize)
{
    if(!isOpen()) {
        qWarning("Qua7zip::open(): file not open");
        return 0;
    }

    if(maxSize <= 0)
        return 0;

    if(m_iCurReadOfs == 0) {
        size_t offset;
	    size_t outSizeProcessed;

	    res = m_manager->readFile( &offset, &outSizeProcessed );
	    if (res != SZ_OK)
		    return 0;
        m_iStartReadOfs = m_iCurReadOfs = offset;
        m_iUnpackedSize = outSizeProcessed;
        m_iOutBufferSize = m_manager->getOutBufferSize();
    }

    if(m_iCurReadOfs >= m_iStartReadOfs + m_iUnpackedSize)
        return 0;
    if(m_iCurReadOfs >= m_iOutBufferSize)
        return 0;

    if(maxSize > m_iUnpackedSize)
        maxSize = m_iUnpackedSize;
    if(maxSize > m_iOutBufferSize - m_iCurReadOfs)
        maxSize = m_iOutBufferSize - m_iCurReadOfs;

    memcpy(data, (char*)(m_manager->getOutBuffer() + m_iCurReadOfs), maxSize);
    m_iCurReadOfs +=  maxSize;

    return maxSize;
}

qint64 Qua7zipFile::writeData(const char *data, qint64 maxSize)
{
    return -1;
}

/*
    class Qua7zip
*/
Qua7zip::Qua7zip( const QString & zipName )
    :   currentFile( 0 )
	,	currentFileIndex( 0 )
	,	blockIndex( 0xFFFFFFFF )
	,	outBuffer( 0 )
	,	outBufferSize( 0 )
    ,   m_7zipName( zipName )
    ,   res( SZ_OK )
    ,   m_mode( mdNotOpen )
{
}

Qua7zip::~Qua7zip()
{
	if(isOpen())
        close();
}

bool Qua7zip::open(OpenMode mode)
{
    if(mode != mdUnpack)
        return false;
    res = SzInit(&archiveStream, &db, &allocImp, &allocTempImp, (const wchar_t*)m_7zipName.utf16() );
    return (res != SZ_OK);
}

void Qua7zip::close()
{
    SzClean( outBuffer, &db, &allocImp, &archiveStream );
    res = SZ_OK;
}

unsigned int
Qua7zip::getEntriesCount() const
{
	return db.Database.NumFiles;
}

bool
Qua7zip::goToFirstFile() const
{
	if( res == SZ_OK )
	{
		currentFile = db.Database.Files;
		currentFileIndex = 0;
		return true;
	}
	else
		return false;
}

bool
Qua7zip::goToNextFile() const
{
	if( res == SZ_OK )
	{
		if( ++currentFileIndex < getEntriesCount() )
		{
			currentFile = db.Database.Files + currentFileIndex;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool
Qua7zip::getCurrentFileInfo( Qua7zipFileInfo & info ) const
{
	if( currentFileIndex < getEntriesCount() )
	{
		info.size     = currentFile->Size;
        info.crc32    = currentFile->FileCRC;
        info.fileName = QString::fromUtf8(currentFile->Name);
		info.isDir    = currentFile->IsDirectory;
		return true;
	}
	else
		return false;
}

QString Qua7zip::getCurrentFileName() const
{
    return ( currentFileIndex < getEntriesCount() ? currentFile->Name : QString() );
}

SZ_RESULT Qua7zip::readFile(	size_t *offset,
	                            size_t *outSizeProcessed )
{
    res = SzReadFile( &archiveStream.InStream, &db, currentFileIndex, 
	    &blockIndex, &outBuffer, &outBufferSize, 
	    offset, outSizeProcessed, 
	    &allocImp, &allocTempImp );
    return res;
}

}// namespace qua7zip
