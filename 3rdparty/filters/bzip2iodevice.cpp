/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
** All rights reserved.
**
** SPDX-License-Identifier: LGPL-2.0-or-later
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "bzip2iodevice.h"

#include <QIODevice>
#include <QString>

#include <bzlib.h>

//
// BZip2IODevice::Private
//
class BZip2IODevice::Private
{
  public:
    Private(BZip2IODevice *parent_, QIODevice *dev, int bs)
  : parent(parent_),
    device(dev),
    iMaxBufSize(1024*1024),
    initialized(false),
    decomressStreamEnd(false),
    blocksize(bs),
    iCurPos(0)
    {
       buffer.resize(iMaxBufSize);
    }
    ~Private()
    {}

    bool initializeCompress();
    qint64 doCompress(const char *in, qint64 iLen);
    bool finishCompress();
    bool initializeDecompress();
    qint64 doDecompress(char *out, qint64 iMaxLen);
    bool finishDecompress();
    QString bzipError2String(int errCode);

    BZip2IODevice  *parent;
    QIODevice      *device;
    QByteArray      buffer;
    unsigned int    iCurPos;
    unsigned int    iMaxBufSize;
    bool            initialized;
    bool            decomressStreamEnd;
    unsigned int    blocksize;
    bz_stream       stream;
};

bool BZip2IODevice::Private::initializeCompress()
{
  if(initialized)
    return true;
  memset(&stream, 0, sizeof(stream));
  int blockSize = (blocksize > 0 && blocksize < 10) ? blocksize : 10;
  int ret = BZ2_bzCompressInit (&stream, blockSize, 0, 0);
  if(ret != BZ_OK) {
    parent->setErrorString(QString(QLatin1String("Error initializing bzip2: %1")).arg(bzipError2String(ret)));
    initialized = false;
  } else {
    initialized = true;
  }

  return initialized;
}

qint64 BZip2IODevice::Private::doCompress(const char *in, qint64 iMaxLen)
{
  int ret = -1;
  if(!initialized) {
    if(parent->errorString().isEmpty())
      parent->setErrorString(QLatin1String("Internal Error - bzip not initialized"));
    finishCompress();
    return -1;
  }
  if(iMaxLen > 2*1024*1024*1024LL) {
    finishCompress();
    parent->setErrorString(QLatin1String("Currently can't handle more than 2GB!"));
    return -1;
  }

  stream.next_in  = const_cast<char*>(in);
  stream.avail_in = iMaxLen;

  do {
    stream.next_out = buffer.data();
    stream.avail_out = buffer.size();

    ret = BZ2_bzCompress(&stream, BZ_RUN);
    if(ret != BZ_RUN_OK) {
      finishCompress();
      parent->setErrorString(QString(QLatin1String("Error executing BZ2_bzCompress: %1")).arg(bzipError2String(ret)));
      return false;
    }
    int iBytesToWrite = buffer.size() - stream.avail_out;
    if(iBytesToWrite) {
      if(device->write(buffer.constData(), iBytesToWrite) != iBytesToWrite) {
        finishCompress();
        parent->setErrorString(QLatin1String("Error writing to QIODevice"));
        return -1;
      }
    }
  } while(stream.avail_in != 0);

  return stream.next_in - in;
}

bool BZip2IODevice::Private::finishCompress()
{
  if(!initialized)
    return true;

  int ret = -1;

  do {
    stream.next_out = buffer.data();
    stream.avail_out = buffer.size();
    ret = BZ2_bzCompress(&stream, BZ_FINISH);
    if(ret != BZ_FINISH_OK && ret != BZ_STREAM_END) {
      finishCompress();
      parent->setErrorString(QString(QLatin1String("Error executing BZ2_bzCompress: %1")).arg(bzipError2String(ret)));
      return false;
    }
    int iBytesToWrite = buffer.size() - stream.avail_out;
    if(device->write(buffer.constData(), iBytesToWrite) != iBytesToWrite) {
      finishCompress();
      parent->setErrorString(QLatin1String("Error writing to QIODevice"));
      return false;
    }
  } while( ret != BZ_STREAM_END );
  BZ2_bzCompressEnd(&stream);

  initialized = false;
  return true;
}

bool BZip2IODevice::Private::initializeDecompress()
{
  if(initialized)
    return true;
  memset(&stream, 0, sizeof(stream));
  int ret = BZ2_bzDecompressInit (&stream, 0, 0);
  if(ret != BZ_OK) {
    parent->setErrorString(QString(QLatin1String("Error initializing bzip2: %1")).arg(bzipError2String(ret)));
    initialized = false;
  } else {
    initialized = true;
  }
  decomressStreamEnd = false;
  return initialized;
}

qint64 BZip2IODevice::Private::doDecompress(char *out, qint64 iMaxLen)
{
  int ret = -1;
  if(!initialized) {
    if(parent->errorString().isEmpty())
      parent->setErrorString(QLatin1String("Internal Error - bzip not initialized"));
    finishDecompress();
    return -1;
  }
  if(decomressStreamEnd)
    return 0;

  if(iMaxLen > 2*1024*1024*1024LL) {
    finishCompress();
    parent->setErrorString(QLatin1String("Currently can't handle more than 2GB!"));
    return -1;
  }

  stream.next_out  = out;
  stream.avail_out = iMaxLen;

  do {
    if(ret != BZ_STREAM_END && stream.avail_in == 0) {
      qint64 iReadCnt = device->read(buffer.data(), buffer.size());
      if(iReadCnt <= 0)
        return -1;
      stream.next_in = buffer.data();
      stream.avail_in = iReadCnt;
    }

    ret = BZ2_bzDecompress(&stream);
    if(ret != BZ_OK && ret != BZ_STREAM_END) {
      parent->setErrorString(QString(QLatin1String("Error executing BZ2_bzCompress: %1")).arg(bzipError2String(ret)));
      finishDecompress();
      return -1;
    }
  } while(stream.avail_out != 0 && ret != BZ_STREAM_END);
  decomressStreamEnd = (ret == BZ_STREAM_END);
  return stream.next_out - out;
}

bool BZip2IODevice::Private::finishDecompress()
{
  if(!initialized)
    return true;
  BZ2_bzDecompressEnd(&stream);

  initialized = false;
  return true;
}

QString BZip2IODevice::Private::bzipError2String( int errCode )
{
  switch( errCode ) {
    case BZ_SEQUENCE_ERROR:
      return QLatin1String("Sequence error");
    case BZ_PARAM_ERROR:
      return QLatin1String("Parameter not correct");
    case BZ_MEM_ERROR:
      return QLatin1String("Memory allocation error");
    case BZ_DATA_ERROR:
      return QLatin1String("Invalid input data");
    case BZ_DATA_ERROR_MAGIC:
      return QLatin1String("Not a bzip2 file");
    case BZ_IO_ERROR:
      return QLatin1String("I/O error");
    case BZ_UNEXPECTED_EOF:
      return QLatin1String("Unexpected end of file");
    case BZ_OUTBUFF_FULL:
      return QLatin1String("Output buffer full");
    case BZ_CONFIG_ERROR:
      return QLatin1String("Wrong config parameter");
    default:
      break;
  };
  return QString::number( errCode );
}

//
// BZip2IODevice
//
BZip2IODevice::BZip2IODevice(QIODevice *dev, int blocksize)
  : d(new Private(this, dev, blocksize))
{}

BZip2IODevice::~BZip2IODevice()
{
  close();
  delete d;
}

void BZip2IODevice::setBufferSize(unsigned int size)
{
  d->iMaxBufSize = size > 1024 ? size : 1024;
  d->buffer.resize(d->iMaxBufSize);
}

void BZip2IODevice::setBlockSize(unsigned int size)
{
  if(isOpen())
    return;
  d->blocksize = size;
}

bool BZip2IODevice::isSequential() const
{
  return true;
}

bool BZip2IODevice::open(OpenMode mode)
{
  if((mode & QIODevice::ReadWrite) == 0) {
    setErrorString(QString(QLatin1String("Unsupported OpenMode %1")).arg(mode));
    return false;
  }
  if((mode & QIODevice::ReadWrite) == QIODevice::ReadWrite) {
    setErrorString(QLatin1String("Can't read and write simultanionously"));
    return false;
  }
  if((mode & QIODevice::ReadOnly) == QIODevice::ReadOnly) {
    if(!d->device) {
      setErrorString(QLatin1String("No QIODevice for reading available"));
      return false;
    }
    if((d->device->openMode() & QIODevice::ReadOnly) == 0) {
      setErrorString(QLatin1String("QIODevice not opened for reading"));
      return false;
    }
    if(!d->initializeDecompress())
      return false;
  }
  else {
    if(!d->device) {
      setErrorString(QLatin1String("No QIODevice for writing available"));
      return false;
    }
    if((d->device->openMode() & QIODevice::WriteOnly) == 0) {
      setErrorString(QLatin1String("QIODevice not opened for writing"));
      return false;
    }
    if(!d->initializeCompress())
      return false;
  }
  setErrorString(QString());
  setOpenMode(mode);
  return QIODevice::open(mode);
}

void BZip2IODevice::close()
{
  if(!isOpen())
    return;
  if((openMode() & QIODevice::ReadOnly) == QIODevice::ReadOnly) {
    d->finishDecompress();
  } else {
    d->finishCompress();
  }
  QIODevice::close();
}

bool BZip2IODevice::atEnd() const
{
  if(!d->device)
    return true;
  return d->device->atEnd() && d->decomressStreamEnd && QIODevice::atEnd();
}

qint64 BZip2IODevice::readData(char * data, qint64 maxSize)
{
  return d->doDecompress(data, maxSize);
}

qint64 BZip2IODevice::writeData(const char * data, qint64 maxSize)
{
  return d->doCompress(data, maxSize);
}
