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

#ifndef BZIP2IODEVICE_H
#define BZIP2IODEVICE_H

#include <QString>
#include <QIODevice>

/* This small class provides a QIODevice interface to (de-)compress
   data with the bzip2 algorithm
   You need to link against bzip lib!
*/
class BZip2IODevice : public QIODevice
{
  public:
    // @dev       - the device to write data to or read data from
    // @blocksize - the blocksize (*100000) to use for compression,
    //              between 1 (fast) and 9 (best compression)
    BZip2IODevice(QIODevice *dev, int blocksize = 5);
    virtual ~BZip2IODevice();

    void setBufferSize(unsigned int size);  // default: 1MB
    void setBlockSize(unsigned int size);   // default: 5

    // QIODevice functions
    virtual bool isSequential() const;
    virtual bool open(OpenMode mode);
    virtual void close();
    virtual bool atEnd() const;
  protected:
    virtual qint64 readData(char * data, qint64 maxSize);
    virtual qint64 writeData(const char * data, qint64 maxSize);
  private:
    BZip2IODevice(const BZip2IODevice&);
    BZip2IODevice operator=(const BZip2IODevice&);
    class Private;
    friend class Private;
    Private * const d;
};

#endif  // BZIP2IODEVICE_H
