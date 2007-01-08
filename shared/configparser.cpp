/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QByteArray>
#include <QFile>
#include <QIODevice>
#include <QBuffer>
#include <QtDebug>

#include "site.h"
#include "configparser.h"

ConfigParser::ConfigParser()
{
}

bool ConfigParser::parseFromFile(const QString &fileName)
{
	QFile configFile(fileName);
	if (!configFile.exists())
  	return false;

	configFile.open(QIODevice::ReadOnly);

  return parse(&configFile);
}

bool ConfigParser::parseFromByteArray(const QByteArray &_ba)
{
  QByteArray ba(_ba);
  QBuffer buf(&ba);

  if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
		return false;
	return parse(&buf);
}

bool ConfigParser::parse(QIODevice *ioDev)
{
	bool inSite;
	bool inPackage;
	Site *site;

	while (!ioDev->atEnd()) {
		QByteArray line = ioDev->readLine().replace("\r\n","");
		if (line.startsWith(";")) 
			continue;
		else if (line.startsWith("@")) {
			QList<QByteArray> cmd = line.split(' ');
			if (cmd[0] == "@format")
				;
			else if(cmd[0] == "@site") {
				site = new Site;
				m_sites.append(site);
				site->setName(cmd[1]);
				qDebug() << "site " << cmd[1] << " detected";
			}
			else if(cmd[0] == "@siteurl")
				site->setURL(cmd[1]);
			else if(cmd[0] == "@sitetype") {
				site->setType(cmd[1]);
			}
			else if(cmd[0] == "@package")
				qDebug() << "package detected";

		}
	}
	return true;
}

