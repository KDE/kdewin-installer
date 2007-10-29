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

#ifndef TARFILTER_H
#define TARFILTER_H

#include <QString>

class QIODevice;
class QTextCodec;
class QByteArray;

/* This small class provides a 'filter' to create/read from a tar stream
   It currently only supports a small piece of the tar specs as it's only
   intended to use it inside the kdewin-installer project
   Feel free to extend it :)
*/
class TarFilter
{
  public:
    enum FileType {
      regular   = '0',
      regular2  = '\0',
      link      = '1',
      reserved  = '2',
      charType  = '3',
      block     = '4',
      directory = '5',
      fifo      = '6',
      reserved2 = '7',
      gnu_longLink = 'K',     // next chunk is a long linkname
      gnu_longname = 'L',     // next chunk is a long filename
    };
    struct FileInformations
    {
      QString   fileName;
      qint64    fileSize;
      int       uid;
      int       gid;
      QString   uname;
      QString   gname;
      time_t    mtime;
      FileType  fileType;
      QFile::Permissions mode;

      FileInformations()
        : uid(1000), gid(100), mtime(-1), fileType(directory), mode(0x0644)
      {}
    };

    // @dev - the device to add data to or get Data from
    // @codec - codec to use for filenames in archive
    TarFilter(QIODevice *dev, QTextCodec *codec = NULL);
    ~TarFilter();

    void setLocaleCodec(QTextCodec *codec); // default: codecForLocale()
    void setBufferSize(unsigned int size);  // default: 1MB
    QString lastError() const;              // last error occoured, not rest on usage

    // add Data to a tar stream
    bool addData(const FileInformations &infos, QIODevice *in);
    // convenience functions for addData()
    bool addFile(const QString &filename, const QString &filenameInArchive = QString());
    bool addData(const QString &filename, const QByteArray &data=QByteArray(), bool bIsDir = false);

    // read data from a tar stream
    bool getData(FileInformations &infos, QIODevice *out);
    // convenience functions for getData()
    bool getData(FileInformations &infos);  // only reads data, for use with getData(QIODevice)
    bool getData(QIODevice *out);
    bool getData(FileInformations &infos, QByteArray &data);

  private:
    TarFilter(const TarFilter&);
    TarFilter operator=(const TarFilter&);

    class Private;
    Private * const d;
};

#endif  // TARFILTER_H
