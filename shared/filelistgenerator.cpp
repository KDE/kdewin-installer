/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
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

#include "filelistgenerator.h"

#include <QtXml>
#include <QtDebug>

class XmlFiles 
{
public:
	XmlFiles(const QXmlAttributes & atts) 
		: directory(atts.value("directory"))
		, pattern(atts.value("pattern"))
		, exclude(atts.value("exclude"))
		, handler(atts.value("handler"))
	{
	}

	QString directory;
	QString pattern;
	QString exclude;
	QString handler;
	friend QDebug operator<<(QDebug, const XmlFiles &);
};

QDebug operator<<(QDebug out, const XmlFiles &c)
{
	out << "\nXmlFiles ("
		<< "directory:" << c.directory
		<< "pattern:" << c.pattern
		<< "exclude:" << c.exclude
		<< "handler:" << c.handler
		<< ")";
	return out;
}

class XmlCompiler
{
public:
	XmlCompiler(const QString &_name) : name(_name)
	{
	}

	XmlCompiler(const QXmlAttributes & atts) : name(atts.value("name"))
	{
	}

	~XmlCompiler()
	{
		qDeleteAll(fileList);
	}
	QString name;// all,mingw,vc80,vc90
	QList<XmlFiles*> fileList;
	friend QDebug operator<<(QDebug,const XmlCompiler &);
};

QDebug operator<<(QDebug out,const XmlCompiler &c)
{
	out << "\nXmlCompiler ("
		<< "name:" << c.name
	;
	foreach(XmlFiles *m, c.fileList)
		out << *m;
	out << ")";
	return out;
}

class XmlPackage 
{
public:
	XmlPackage(const QString &_type) : type(_type)
	{
	}

	XmlPackage(const QXmlAttributes & atts) : type(atts.value("type"))
	{
	}

	~XmlPackage()
	{
		qDeleteAll(compilerList);
	}

	QString type; // runtime, development, documentation, source
	QMap<QString,XmlCompiler*> compilerList;
	friend QDebug operator<<(QDebug out,const XmlPackage &c);
};

QDebug operator<<(QDebug out,const XmlPackage &c)
{
	out << "\nXmlPackage ("
		<< "type:" << c.type
	;
	foreach(XmlCompiler *m, c.compilerList)
		out << *m;
	out << ")";
	return out;
}

class XmlPackageScheme
{
public:
	XmlPackageScheme(const QString &_name) : name(_name)
	{
	}
	XmlPackageScheme(const QXmlAttributes & atts) 
		: name(atts.value("name")), alias(atts.value("alias"))
	{
	}
	~XmlPackageScheme()
	{
		qDeleteAll(packageList);
	}
	QString name; // kde,qt
	QString alias; // kde,qt
	QMap<QString,XmlPackage*> packageList;
	friend QDebug operator<<(QDebug,const XmlPackageScheme &);
};

QDebug operator<<(QDebug out,const XmlPackageScheme &c)
{
	out << "\nXmlPackageScheme ("
		<< "name:" << c.name
	;
	foreach(XmlPackage *m, c.packageList)
		out << *m;
	out << ")";
	return out;
}

class XmlData 
{
public:
	~XmlData()
	{
		qDeleteAll(schemeList);
	}
	QMap<QString,XmlPackageScheme*> schemeList;
};

QDebug operator<<(QDebug out,const XmlData &c)
{
	out << "\nXmlData ("
	;
	foreach(XmlPackageScheme *m, c.schemeList)
		out << *m;
	out << ")";
	return out;
}

class MyXmlHandler : public QXmlDefaultHandler
{
public:
	MyXmlHandler(XmlData *data) : m_data(data)
	{
	}
	
	bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts ) 
	{
		inElement = true;
		element = qName;
		if (qName == "packagescheme")
		{
			m_scheme = new XmlPackageScheme(atts);
			m_data->schemeList[atts.value("name")] = m_scheme;
		}
		else if (qName == "package")
		{
			m_package = new XmlPackage(atts);
			m_scheme->packageList[atts.value("type")] = m_package;
		}
		else if (qName == "compiler")
		{
			m_compiler = new XmlCompiler(atts);
			m_package->compilerList[atts.value("name")] = m_compiler;
		}
		else if (qName == "files")
		{
			m_files = new XmlFiles(atts);
			m_compiler->fileList.append(m_files);
		}
		return true;
	}
	
	bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName )
	{
		inElement = false;
		return true;
	}

	bool characters ( const QString & ch )  
	{
		if  (!inElement)	
			return true;
		// handle in element data
		return true;
	}

	protected:
		QString element;
		bool inElement;
		XmlPackageScheme *m_scheme;
		XmlPackage *m_package;
		XmlCompiler *m_compiler;
		XmlFiles *m_files;
		XmlData *m_data;
};

FileListGenerator::FileListGenerator() : m_verbose(false)
{
	m_data = new XmlData;
}

FileListGenerator::~FileListGenerator()
{
	delete m_data;
}


bool FileListGenerator::parse(const QString &fileName)
{
	QXmlDefaultHandler *handler = new MyXmlHandler(m_data);	
	QXmlSimpleReader xmlReader;
	QFile file(fileName);
	xmlReader.setContentHandler(handler);
	xmlReader.setErrorHandler(handler);

	QXmlInputSource *source = new QXmlInputSource(&file);
	bool ok = xmlReader.parse(source);
	return ok;
}
	
bool FileListGenerator::generatePackageFileList(QList<InstallFile> &fileList, Packager::Type type, const QString compilerType, const QString &root)
{
	fileList.clear();
	QString packageType;
	if (type == Packager::BIN)
		packageType = "runtime";
	else if (type == Packager::LIB)
		packageType = "development";
	else if (type == Packager::DOC)
		packageType = "documentation";
	else if (type == Packager::SRC)
		packageType = "source";
	else if (type == Packager::ALL)
		packageType = "all";

	// find aliased entry 
	XmlPackageScheme *ps = m_data->schemeList["default"];
	if (!ps->alias.isEmpty())
		ps = m_data->schemeList[ps->alias];

	// add compiler independent files
	XmlPackage *p = ps->packageList[packageType];
	if (!p)
		return false;
	XmlCompiler *c = p->compilerList["all"];
	foreach(XmlFiles *f, c->fileList)
	{
		if (m_verbose)
			qDebug() << *f;					
		if (!f->handler.isEmpty())
			parseQtIncludeFiles(fileList, root, f->directory,  f->pattern, f->exclude);
		else
			generateFileList(fileList, root, f->directory,  f->pattern, f->exclude);
	}

	// add compiler specific files 
	p = ps->packageList[packageType];
	if (!p)
		return false;
	c = p->compilerList[compilerType];
	if (!c)
	{
		qDebug() << fileList;
		return true;
	}

	foreach(XmlFiles *f, c->fileList)
	{
		if (m_verbose)
			qDebug() << *f;					

		if (!f->handler.isEmpty())
			parseQtIncludeFiles(fileList, root, f->directory,  f->pattern, f->exclude);
		else
			generateFileList(fileList, root, f->directory,  f->pattern, f->exclude);
	}
	qDebug() << fileList;
	return true;
}

