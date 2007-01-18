/*
 * qua7zip.cpp - unpack 7zip files
 * Copyright (C) 2007  Igor Mironchick <imironchick@gmail.com>
 * Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


/*!	\file	qua7zip.h
	\brief	C++ interface for Szip.

    The interface is derived from quazip (quazip.sf.net)
    - special thx to Sergey A. Tachenov
*/

#ifndef _QUA7ZIP_H_
#define _QUA7ZIP_H_

#include <QString>
#include <QIODevice>

#include "lzma_sdk/7zip/Archive/7z_C/7zIn.h"
#include "lzma_sdk/7zip/Archive/7z_C/7zCrc.h"
#include "lzma_sdk/7zip/Archive/7z_C/7zExtract.h"

#include "utils.h"

namespace qua7zip {

//
// struct Qua7zipFileInfo
//

//! Information about file.
struct Qua7zipFileInfo {
	//! Name of file.
	QString fileName;
	//! Is current file a dir?
	bool isDir;
	//! Size of (unpacked) file
    qint64 size;
    //! CRC32 (?) of (unpacked) file
    unsigned int crc32;

	Qua7zipFileInfo()
		:	isDir( false )
        ,   size( 0 )
        ,   crc32( 0 )
	{};
};// struct Qua7zipFileInfo

//
// class Qua7zip
//

class Qua7zip;
class Qua7zipFile : public QIODevice
{
public:
    Qua7zipFile(Qua7zip *manager, QObject *parent = 0);
    ~Qua7zipFile();

    /*!
        Same as Qua7zip::get7zipName()
    */
    QString get7zipName() const;
    /*!
        Same as Qua7zip::getCurrentFileName()
    */
    QString getFileName() const;
    /*!
        Same as Qua7zip::getCurrentFileInfo()
    */
    bool getFileInfo(Qua7zipFileInfo &info) const;
    // QIODevice
    virtual bool open(OpenMode mode);
    virtual void close();
    virtual bool isSequential() const;
protected:
    // QIODevice
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

    qint64 m_iStartReadOfs;
    qint64 m_iCurReadOfs;
    qint64 m_iUnpackedSize;
    qint64 m_iOutBufferSize;

    Qua7zip *m_manager;
	SZ_RESULT res;
};// class Qua7zipFile

//! Main manager of Szip archives.
class Qua7zip
{
public:
    enum OpenMode{ mdNotOpen, mdUnpack };
public:
	Qua7zip( const QString &zipName );
	~Qua7zip();

    //! Opens the 7zip file
    bool open(OpenMode mode);
    //! Closes the 7zip file.
    void close();
    //! returns true if OpenMode != mdNotOpen
    bool isOpen() const {return m_mode!=mdNotOpen;}
    //! returns the current OpenMode
    OpenMode getMode() const { return m_mode; }

    //! Return last operation status.
	SZ_RESULT status() const;
	//! Reinitialization of manager.
	bool reInit( const QString & zipName );

    //! Return number of entries in the archive.
	unsigned int getEntriesCount() const;
	/*!
		Set iterator to the first entrie in the archive.
		And return TRUE if operation completed successfully.
	*/
	bool goToFirstFile() const;
	/*!
		Set iterator to the next entrie in the archive.
		And return TRUE if operation completed successfully.
	*/
	bool goToNextFile() const;
	/*!
		Set iterator to the first entries in the archive.
		And return TRUE if operation completed successfully.
	*/
	bool getCurrentFileInfo( Qua7zipFileInfo & info ) const;

    /*!
        Returns the filename of the archive
    */
    const QString get7zipName() { return m_7zipName; }
    /*!
        Returns the name of the current file in archive
    */
    QString getCurrentFileName() const;

private:
    SZ_RESULT readFile(	size_t *offset,
	                    size_t *outSizeProcessed );
    Byte *getOutBuffer() { return outBuffer; }
    size_t getOutBufferSize() const { return outBufferSize; }
protected:

    QString m_7zipName;
    OpenMode m_mode;

private:
	//! Close all active resources.
	void clear();

// DATA.
	CArchiveDatabaseEx db;
	CFileInStream archiveStream;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	SZ_RESULT res;
	UInt32 blockIndex;
	Byte * outBuffer;
	size_t outBufferSize;

	mutable CFileItem * currentFile;
	mutable size_t currentFileIndex;

// Disabling of implementation and others.
	Qua7zip( const Qua7zip & o );
	Qua7zip & operator=( const Qua7zip & o );

    friend class Qua7zipFile;
};// class Qua7zip

} // namespace qua7zip

#endif // _QUA7ZIP_H_
