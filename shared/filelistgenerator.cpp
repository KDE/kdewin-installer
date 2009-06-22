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
		, regexp(atts.value("regexp"))
		, handler(atts.value("handler"))
	{
	}

	QString directory;
	QString pattern;
	QString exclude;
	QString handler;
	QString regexp;
	friend QDebug operator<<(QDebug, const XmlFiles &);
};

QDebug operator<<(QDebug out, const XmlFiles &c)
{
	out << "\nXmlFiles ("
		<< "directory:" << c.directory
		<< "pattern:" << c.pattern
		<< "regexp:" << c.regexp
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

class XmlPart
{
public:
	XmlPart(const QString &_name) : name(_name)
	{
	}

	XmlPart(const QXmlAttributes & atts) : name(atts.value("name"))
	{
	}

	~XmlPart()
	{
		qDeleteAll(compilerList);
		qDeleteAll(fileList);
	}

	QString name; // runtime, development, documentation, source
	// compiler is optional 
	QMap<QString,XmlCompiler*> compilerList;
	QList<XmlFiles*> fileList;
	friend QDebug operator<<(QDebug out,const XmlPart &c);
};

QDebug operator<<(QDebug out,const XmlPart &c)
{
	out << "\nXmlPart ("
		<< "name:" << c.name
	;
	foreach(XmlCompiler *m, c.compilerList)
		out << *m;
	foreach(XmlFiles *m, c.fileList)
		out << *m;
	out << ")";
	return out;
}

class XmlPackage 
{
public:
	XmlPackage(const QString &_name) : name(_name)
	{
	}

	XmlPackage(const QXmlAttributes & atts) : name(atts.value("name"))
	{
	}

	~XmlPackage()
	{
		qDeleteAll(partList);
	}

	QString name; 
	QString description; 
	QMap<QString,XmlPart*> partList;
	friend QDebug operator<<(QDebug out,const XmlPackage &c);
};

QDebug operator<<(QDebug out,const XmlPackage &c)
{
	out << "\nXmlPackage ("
		<< "name:" << c.name
	;
	foreach(XmlPart *m, c.partList)
		out << *m;
	out << ")";
	return out;
}

class XmlModule
{
public:
	XmlModule(const QString &_name) : name(_name)
	{
	}
	XmlModule(const QXmlAttributes & atts) 
		: name(atts.value("name")), alias(atts.value("alias"))
	{
	}
	~XmlModule()
	{
		qDeleteAll(packageList);
	}
	QString name; // kde,qt
	QString alias; // kde,qt
	QMap<QString,XmlPackage*> packageList;
	friend QDebug operator<<(QDebug,const XmlModule &);
};

QDebug operator<<(QDebug out,const XmlModule &c)
{
	out << "\nXmlModule ("
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
		qDeleteAll(moduleList);
	}
	QMap<QString,XmlModule*> moduleList;
};

QDebug operator<<(QDebug out,const XmlData &c)
{
	out << "\nXmlData ("
	;
	foreach(XmlModule *m, c.moduleList)
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
		if (qName == "module")
		{
			m_module = new XmlModule(atts);
			m_data->moduleList[atts.value("name")] = m_module;
		}
		else if (qName == "package")
		{
			m_parent = m_last;
			m_package = new XmlPackage(atts);
			m_module->packageList[atts.value("name")] = m_package;
		}
		else if (qName == "part")
		{
			m_parent = m_last;
			m_part = new XmlPart(atts);
			m_package->partList[atts.value("name")] = m_part;
		}
		else if (qName == "compiler")
		{
			m_parent = m_last;
			m_compiler = new XmlCompiler(atts);
			m_part->compilerList[atts.value("name")] = m_compiler;
		}
		else if (qName == "files")
		{
			m_parent = m_last;
			m_files = new XmlFiles(atts);
			m_compiler->fileList.append(m_files);
		}
		m_last = qName;
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
		if (element == "shortDescription")
			m_package->description = ch;
	
		return true;
	}

	bool error( const QXmlParseException & exception )
	{
		qDebug() << exception.lineNumber() << exception.columnNumber() << exception.message();
		return  true;
	}

	QString errorString ()
	{
	}

	bool fatalError ( const QXmlParseException & exception )
	{
		qCritical() << exception.lineNumber() << exception.columnNumber() << exception.message();
		return  false;
	}

	bool warning ( const QXmlParseException & exception )
	{
		qWarning() << exception.lineNumber() << exception.columnNumber() << exception.message();
		return  true;
	}

	protected:
		QString element;
		bool inElement;
		QString m_parent;
		QString m_last;
		XmlModule *m_module;
		XmlPackage *m_package;
		XmlPart *m_part;
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
	qDebug() << *m_data;
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
	XmlModule *m = m_data->moduleList["kde"];
	if (!m->alias.isEmpty())
		m = m_data->moduleList[m->alias];

	// add compiler independent file
	XmlPackage *p = m->packageList["default"];
	if (!p)
		return false;

	XmlPart *part = p->partList[packageType];
	if (!part)
		return false;
		
	XmlCompiler *c = part->compilerList["all"];
	foreach(XmlFiles *f, c->fileList)
	{
		if (m_verbose)
			qDebug() << *f;					
		if (f->handler == "parseQtIncludeFiles")
			parseQtIncludeFiles(fileList, root, f->directory,  f->pattern, f->exclude);
		else
			generateFileList(fileList, root, f->directory,  f->pattern, f->exclude);
	}

	// add compiler specific files 
	c = part->compilerList[compilerType];
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

