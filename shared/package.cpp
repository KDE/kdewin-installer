/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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

#include "package.h"
#include "packagelist.h"

#include <QtGui>
#include <QtNetwork>

QString Package::baseURL = "http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/";

Package::Package()
{
	installedLIB = false;
	installedBIN = false;
	installedDOC = false;
	installedSRC = false;
}

Package::Package(QString const &_name, QString const &_version) 
{ 
	name = _name; version = _version; 
	installedLIB = false;
	installedBIN = false;
	installedDOC = false;
	installedSRC = false;
}


const QString Package::getFileName(Package::Type type)
{
	switch (type) {
		case BIN: return name + "-" + version + "-bin.zip"; 
		case LIB: return name + "-" + version + "-lib.zip"; 
		case SRC: return name + "-" + version + "-doc.zip"; 
		case DOC: return name + "-" + version + "-src.zip"; 
		default:  return "";
	}
}

const QString Package::getURL(Package::Type type)
{
	switch (type) {
		case BIN: return baseURL + name + "-" + version + "-bin.zip"; 
		case LIB: return baseURL + name + "-" + version + "-lib.zip"; 
		case SRC: return baseURL + name + "-" + version + "-doc.zip"; 
		case DOC: return baseURL + name + "-" + version + "-src.zip"; 
		default:  return "";
	}
}

void Package::setType(const QString &typeString)
{
	if (typeString == "bin")
		installedBIN = true;
	else if (typeString == "lib")
		installedLIB = true;
	else if (typeString == "src")
		installedSRC = true;
	else if (typeString == "doc")
		installedDOC = true;
}

QString Package::toString(bool mode, const QString &delim) 
{ 
	QString result = name + delim + version;
	QString installedTypes = getTypeAsString();
	if (installedTypes != "")
	 	result += "   ( installed =" + getTypeAsString() + ")";
	 return result; 
}

const QString Package::getTypeAsString()
{
	QString types;
	if (installedBIN) 
		types += " bin ";
	if (installedLIB) 
		types += " lib ";
	if (installedSRC) 
		types += " src ";
	if (installedDOC) 
		types += " doc ";
	return types;
}

bool Package::setFromVersionFile(const QString &str)
{
	QString verString = str;
	verString.replace(".ver","");
	int i = verString.indexOf("-");
	int j = verString.lastIndexOf("-");
	QString name = verString.left(i);
	QString version = verString.mid(i+1,j-1-i);
	QString type = verString.mid(j+1);
	setName(name);
	setVersion(version);
	setType(type);
	return true;
}

/*
bool Package::updateFromVersionFile(const QString &str)
{
	qDebug() << str;
	QString verString = str;
	verString.replace(".ver","");
	int i = verString.indexOf("-");
	int j = verString.lastIndexOf("-");
	QString name = verString.left(i);
	QString version = verString.mid(i+1,j-1-i+1);
	QString type = verString.right(j+1);
	return true;
}
*/

void Package::addInstalledTypes(const Package &pkg)
{
	installedLIB = pkg.installedLIB ? pkg.installedLIB : installedLIB ;
	installedBIN = pkg.installedBIN ? pkg.installedBIN : installedBIN ;
	installedDOC = pkg.installedDOC ? pkg.installedDOC : installedDOC ;
	installedSRC = pkg.installedSRC ? pkg.installedSRC : installedSRC ;
}

void Package::logOutput()
{
}

