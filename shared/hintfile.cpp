/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#include <QFile>
#include <QBuffer>
#include "hintfile.h"

bool HintFile::parse(QIODevice *ioDev, HintFileType &hf)
{
    bool ldesc = false;
	QString longDesc;
    while (!ioDev->atEnd()) {
        QByteArray line = ioDev->readLine().replace("\n","");

        if (line.startsWith("sdesc: ")) {
            hf.shortDesc = line.mid(8,line.length()-8-1).trimmed().replace("\"","");        
            continue;
        }
        if (line.startsWith("ldesc: ")) {
            ldesc = true;
            longDesc = line.mid(8);
        }
        else if (line.startsWith("category: ")) {
            ldesc = false;
            hf.categories = line.replace("category: ","").trimmed();
        }    
        else if (line.startsWith("requires: ")) {
            ldesc = false;
            hf.requires = line.replace("requires: ","").trimmed();
        }
        else if (ldesc) {
			longDesc += "\n" + line;
        }
    }
	hf.longDesc = longDesc.trimmed().replace("\"","");

	return true;
}

bool HintFile::parse(const QString &hintFile, HintFileType &hf)
{
    QFile file(hintFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return false;

	return parse(&file,hf);
}


bool HintFile::parse(const QByteArray &_ba, HintFileType &hf)
{
    QByteArray ba(_ba);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;
	return parse(&buf,hf);
}


bool HintFile::parseHintFile(const QByteArray &ba, HintFileType &hfd)
{
    HintFile a;
    return a.parse(ba,hfd);
}

bool HintFile::parseHintFile(const QString &file, HintFileType &hfd)
{
    HintFile a;
    return a.parse(file,hfd);
}
