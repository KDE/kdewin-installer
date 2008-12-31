/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QFileInfo>
#include <QDateTime>
#include <QTextCodec>

#include <time.h>

#include "tarfilter.h"

#ifndef Q_OS_WIN
 #include <pwd.h>
 #include <grp.h>
#endif

// TODO: proper cleanup states on error
//       add full support than the default one used by tar
//       add support for special devices & links

//
// TarFilter::Private
//
class TarFilter::Private
{
  public:
    Private(QIODevice *dev, QTextCodec *c)
  : device(dev),
    codec(c ? c : QTextCodec::codecForLocale()),
    iBufSize(1024*1024),
    nextChunkSize(-1)
  {}

    struct tar_posix_header
    {
      char name[100];
      char mode[8];
      char uid[8];
      char gid[8];
      char size[12];
      char mtime[12];
      char chksum[8];
      char typeflag;
      char linkname[100];
      char magic[6];
      char version[2];
      char uname[32];
      char gname[32];
      char devmajor[8];
      char devminor[8];
      char prefix[155];
      char unused[12];  // to align to 512bytes
    };
    bool fi2tph(const FileInformations &infos);     // converts FileInformations to tar_posix_header and adds them to QIODevice
    bool addHeader(const FileInformations &infos);  // adds a complete header for FileInformations

    bool tph2fi(FileInformations &infos);  // reads a tar_posix_header and convert it to FileInformations
    bool getHeader(FileInformations &infos);          // reads a header and converts them to FileInformations
    bool checkDevice(QIODevice *in, QIODevice *out);
    bool copyData(QIODevice *in, QIODevice *out, qint64 iSize);

    QIODevice   *device;
    QTextCodec  *codec;
    QString      lastError;
    unsigned int iBufSize;
    qint64       nextChunkSize;
};

bool TarFilter::Private::fi2tph(const FileInformations &infos)
{
  QByteArray fn = codec->fromUnicode(infos.fileName);
#ifdef Q_OS_WIN
  if(fn.size() > 3 && fn[1] == ':' && (fn[2] == '\\' || fn[2] == '/'))
     fn = fn.mid(3);
#endif
  if(fn.startsWith('/'))
    fn = fn.mid(1);

  long unsigned int fileSize = infos.fileType == directory ? 0 : infos.fileSize;
  long unsigned int mtime    = infos.mtime != -1 ? infos.mtime : time(NULL);
  tar_posix_header hdr;
  memset(&hdr, 0, sizeof(hdr));
  QByteArray fn_part1 = fn.left(sizeof(hdr.name));
  memcpy(hdr.name,  fn_part1.constData(), fn_part1.size());
  sprintf(hdr.mode,   "%07x",     0x0fff & infos.mode);
  sprintf(hdr.uid,    "%07o",     infos.uid);
  sprintf(hdr.gid,    "%07o",     infos.gid);
  sprintf(hdr.size,   "%011lo",   fileSize);
  sprintf(hdr.mtime,  "%011lo",   mtime);
  memset(hdr.chksum,  ' ',        sizeof(hdr.chksum)); // 8 * ' '
  hdr.typeflag = infos.fileType;
  //memset(hdr.linkname, 0,          sizeof(hdr.linkname));
  memcpy(hdr.magic,    "ustar  ",  sizeof(hdr.magic) + sizeof(hdr.version));
#ifdef Q_OS_WIN
  //memset(hdr.uname,    0,          sizeof(hdr.uname));
  //memset(hdr.gname,    0,          sizeof(hdr.gname));
#else
  qstrncpy(hdr.uname, codec->fromUnicode(infos.uname).constData(), sizeof(hdr.uname));
  qstrncpy(hdr.gname, codec->fromUnicode(infos.gname).constData(), sizeof(hdr.gname));
#endif
  //memset(hdr.devmajor, 0,          sizeof(hdr.devmajor));
  //memset(hdr.devminor, 0,          sizeof(hdr.devminor));
  //memset(hdr.prefix,   0,          sizeof(hdr.prefix));

  int checksum = 0;
  const unsigned char *ptr = (const unsigned char*)(&hdr);
  for (unsigned i = 0; i < sizeof(hdr); i++)
    checksum += ptr[i];
  sprintf(hdr.chksum, "%06o", checksum);

  if(!device->write((const char*)&hdr, sizeof(hdr))) {
    lastError = QLatin1String("Error writing to QIODevice");
    return false;
  }
  return true;
}

bool TarFilter::Private::addHeader(const FileInformations &infos)
{
  // TODO: do some sanity checks (e.g. for filesize)

  QByteArray fn = codec->fromUnicode(infos.fileName);
#ifdef Q_OS_WIN
  if(fn.size() > 3 && fn[1] == ':' && (fn[2] == '\\' || fn[2] == '/'))
     fn = fn.mid(3);
#endif
  if(fn.startsWith('/'))
    fn = fn.mid(1);
  if(fn.size() > 512) {
    lastError = QLatin1String("Can't handle filenames with more then 512 bytes");
    return false;
  }
  if(fn.size() > 100) {
    FileInformations longlink;
    longlink.fileName = QLatin1String("././@LongLink");
    longlink.fileType = gnu_longname;
    longlink.fileSize = strlen(fn.constData()) + 1; // including '\0' ?
    longlink.mtime    = 0;
    longlink.uid      = 0;
    longlink.gid      = 0;
    longlink.mode     = 0;
#ifndef Q_OS_WIN
    struct passwd *p = getpwuid(longlink.uid);
    if(p)
      longlink.uname = codec->toUnicode(p->pw_name);
    struct group *g = getgrgid(longlink.gid);
    if(g)
      longlink.gname = codec->toUnicode(g->gr_name);
#endif
    fi2tph(longlink);
    QByteArray ba(512, '\0');
    qstrncpy(ba.data(), fn.constData(), ba.size());
    if(!device->write(ba.constData(), ba.size())) {
      lastError = QLatin1String("Error writing to QIODevice");
      return false;
    }
  }
  if(!fi2tph(infos))
    return false;

  return true;
}

bool TarFilter::Private::tph2fi(FileInformations &infos)
{
  Private::tar_posix_header hdr;
  do {
    int iBytesRead = device->read((char*)&hdr, sizeof(hdr));
    if(iBytesRead != sizeof(hdr)) {
      if(iBytesRead == 0 && device->atEnd()) {
        lastError = QString();
        return false;
      }
      lastError = QString(QLatin1String("Error reading from QIODevice! needed: %1, got: %2")).arg(sizeof(hdr)).arg(iBytesRead);
      return false;
    }
  } while ( memcmp(hdr.magic, "\0\0\0\0\0\0", sizeof(hdr.magic)) == 0 );
  if(memcmp(hdr.magic, "ustar  ", sizeof(hdr.magic))) {
    lastError = QLatin1String("The archive is not supported");
    return false;
  }
  int checksum = QByteArray(hdr.chksum, sizeof(hdr.chksum)).toUInt(NULL, 8);
  memset(hdr.chksum, ' ', sizeof(hdr.chksum));
  int realChecksum = 0;
  const unsigned char *ptr = (const unsigned char*)(&hdr);
  for (unsigned i = 0; i < sizeof(hdr); i++)
    realChecksum += ptr[i];
  if(checksum != realChecksum) {
    lastError = QString(QLatin1String("Checksum does not match! needed: %1, got: %2")).arg(checksum).arg(realChecksum);
    return false;
  }

  QByteArray ba(hdr.name,  sizeof(hdr.name));
  ba.resize(qstrlen(ba.constData()));
  infos.fileName = codec->toUnicode(ba);
  infos.fileSize = QByteArray(hdr.size,  sizeof(hdr.size)).toUInt(NULL, 8);
  infos.mode     = static_cast<QFile::Permissions>(QByteArray(hdr.mode,  sizeof(hdr.mode)).toUInt(NULL, 16));
  infos.uid      = QByteArray(hdr.uid,   sizeof(hdr.uid)).toUInt(NULL, 8);
  infos.gid      = QByteArray(hdr.gid,   sizeof(hdr.gid)).toUInt(NULL, 8);
  infos.mtime    = QByteArray(hdr.mtime, sizeof(hdr.mtime)).toUInt(NULL, 8);
  infos.fileType = (FileType)hdr.typeflag;
  infos.uname    = codec->toUnicode(hdr.uname, sizeof(hdr.uname));
  infos.gname    = codec->toUnicode(hdr.gname, sizeof(hdr.gname));

  return true;
}

bool TarFilter::Private::getHeader(FileInformations &infos)
{
  if(!tph2fi(infos))
    return false;
  switch(infos.fileType) {
    case regular:
    case regular2:
    case directory:
    case gnu_longname:
      break;
    default:
      lastError = QString(QLatin1String("Unsupported fileType %1")).arg(infos.fileType);
      return false;
  }
  if(infos.fileType == gnu_longname) {
    QByteArray fn = device->read(512);
    if(fn.size() != 512) {
      lastError = QLatin1String("Unexpected end of inputstream");
      return false;
    }
    if(!tph2fi(infos))
      return false;
    int len = qstrlen(fn);
    QString fileName = codec->toUnicode(fn.constData(), len);
    infos.fileName = fileName;
  }

  return true;
}

bool TarFilter::Private::checkDevice(QIODevice *in, QIODevice *out)
{
  if(!in) {
    lastError = QLatin1String("No QIODevice for reading available");
    return false;
  }
  if((in->openMode() & QIODevice::ReadOnly) == 0) {
    lastError = QLatin1String("QIODevice not opened for reading");
    return false;
  }

  if(!out) {
    lastError = QLatin1String("No QIODevice for writing available");
    return false;
  }
  if((out->openMode() & QIODevice::WriteOnly) == 0) {
    lastError = QLatin1String("QIODevice not opened for writing");
    return false;
  }
  return true;
}

bool TarFilter::Private::copyData(QIODevice *in, QIODevice *out, qint64 iSize)
{
  if(iSize <= 0)
    return true;

  int iBytesToRead = 1;
  while(iSize > 0) {
    iBytesToRead = iSize > iBufSize ? iBufSize : iSize;

    QByteArray ba = in->read(iBytesToRead);
    if(ba.size() == 0) {
      lastError = QString(QLatin1String("Could not read %1 bytes from QIODevice")).arg(iBytesToRead);
      return false;
    }
    if(out->write(ba) != ba.size()) {
      lastError = QString(QLatin1String("Could not write %1 bytes to QIODevice")).arg(ba.size());
      return false;
    }
    iSize -= ba.size();
  }
  return true;
}

//
// TarFilter
//
TarFilter::TarFilter(QIODevice *dev, QTextCodec *codec)
  : d(new Private(dev, codec))
{}

TarFilter::~TarFilter()
{
  delete d;
}

void TarFilter::setLocaleCodec(QTextCodec *codec)
{
  d->codec = codec ? codec : QTextCodec::codecForLocale();
}

void TarFilter::setBufferSize(unsigned int size)
{
  d->iBufSize = size > 1024 ? size : 1024;
}

bool TarFilter::error() const
{
  return !d->lastError.isEmpty();
}

QString TarFilter::lastError() const
{
  return d->lastError;
}

bool TarFilter::addFile(const QString &filename, const QString &filenameInArchive)
{
  QFileInfo fi(filename);
  if(!fi.exists()) {
    d->lastError = QString(QLatin1String("File %1 does not exist")).arg(filename);
    return false;
  }
  if(!(fi.isDir() || fi.isFile())) {
    d->lastError = QString(QLatin1String("%1 isn't a directory or a regular file")).arg(filename);
    return false;
  }

  FileInformations fInfos;
  fInfos.fileName = (filenameInArchive.isEmpty() ? filename : filenameInArchive);
  fInfos.fileSize = fi.isDir() ? 0 : fi.size();
  fInfos.mode     = fi.permissions() & 0x0fff;
#ifdef Q_OS_WIN
  fInfos.uid      = 1000;
  fInfos.gid      = 100;
#else
  fInfos.uid      = fi.ownerId();
  fInfos.gid      = fi.groupId();
  fInfos.uname    = fi.owner();
  fInfos.gname    = fi.group();
#endif
  fInfos.mtime    = fi.lastModified().toTime_t();
  fInfos.fileType = fi.isDir() ? directory : regular;

  if(fi.isDir()) {
    QBuffer buf;
    buf.open(QIODevice::ReadOnly);
    return addData(fInfos, &buf);
  }
  QFile f(filename);
  if(!f.open(QIODevice::ReadOnly)) {
    d->lastError = QString(QLatin1String("Error opening %1 for reading")).arg(filename);
    return false;
  }
  return addData(fInfos, &f);
}

bool TarFilter::addData(const QString &filename, const QByteArray &data, bool bIsDir)
{
  FileInformations fi;
  fi.fileName = filename;
  fi.fileSize  = bIsDir ? 0 : data.size();
  fi.mode     = static_cast<QFile::Permissions>(bIsDir ? 0x0755 : 0x0644);
  fi.fileType = bIsDir ? directory : regular;
#ifndef Q_OS_WIN
  fi.uid      = getuid();
  fi.gid      = getgid();
  struct passwd *p = getpwuid(fi.uid);
  if(p)
    fi.uname = d->codec->toUnicode(p->pw_name);
  struct group *g = getgrgid(fi.gid);
  if(g)
    fi.gname = d->codec->toUnicode(g->gr_name);
#endif

  QBuffer buf(const_cast<QByteArray*>(&data));
  if(!buf.open(QIODevice::ReadOnly)) {
    d->lastError = QLatin1String("Internal Error - can't open QBuffer for reading!");
    return false;
  }
  return addData(fi, &buf);
}

bool TarFilter::addData(const FileInformations &fi, QIODevice *in)
{
  if(!d->checkDevice(in, d->device))
    return false;

  if(!d->addHeader(fi))
    return false;

  if(fi.fileSize) {
    if(!d->copyData(in, d->device, fi.fileSize))
      return false;

    int align = fi.fileSize % 512;
    if(align) {
      align = 512 - align;
      QByteArray ba2(align, '\0');
      d->device->write(ba2.constData(), align);
    }
  }
  return true;
}

bool TarFilter::writeEOS()
{
  const int size = sizeof(TarFilter::Private::tar_posix_header);
  QByteArray ba(size, '\0');
  return(d->device->write(ba) == size);
}

bool TarFilter::getData(FileInformations &infos)
{
  d->nextChunkSize = -1;
  if(!d->getHeader(infos))
    return false;
  d->nextChunkSize = infos.fileSize;
  return true;
}

bool TarFilter::getData(QIODevice *out)
{
  if(!d->checkDevice(d->device, out))
    return false;

  if(d->nextChunkSize < 0) {
    d->lastError = QLatin1String("No informations about next chunk size. Did you call getData(FileInformations &infos) before?");
    return false;
  }

  if(!d->copyData(d->device, out, d->nextChunkSize))
      return false;
  int align = d->nextChunkSize % 512;
  if(align)
    d->device->read(512 - align);

  d->nextChunkSize = -1;
  return true;
}

bool TarFilter::getData(FileInformations &fi, QByteArray &data)
{
  if(!getData(fi))
    return false;

  QBuffer buf(&data);
  buf.open(QIODevice::WriteOnly);
  return getData(&buf);
}

bool TarFilter::getData(FileInformations &fi, QIODevice *out)
{
  if(!getData(fi))
    return false;

  return getData(out);
}
