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

// check for double used files 
// fix  

#include "xmltemplatepackager.h"

#include <QFile>
#include <QtXml>
#include <debug.h>

class XmlFiles 
{
public:
    XmlFiles(const QXmlAttributes & atts) 
        : compiler(atts.value("compiler"))
        , directory(atts.value("directory"))
        , exclude(atts.value("exclude"))
        , handler(atts.value("handler"))
        , include(atts.value("include"))
    {
    }

    QString compiler;
    QString directory;
    QString exclude;
    QString handler;
    QString include;
    QStringList fileList;
    friend QDebug operator<<(QDebug, const XmlFiles &);
};

QDebug operator<<(QDebug out, const XmlFiles &c)
{
    QDEBUG_CLASS_START(out,XmlFiles,c)
    QDEBUG_CLASS_MEMBER(compiler)
    QDEBUG_CLASS_MEMBER(directory)
    QDEBUG_CLASS_MEMBER(include)
    QDEBUG_CLASS_MEMBER(exclude)
    QDEBUG_CLASS_MEMBER(handler)
    QDEBUG_CLASS_END()
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
        qDeleteAll(fileList);
    }

    QString name; // runtime, development, documentation, source
    QList<XmlFiles*> fileList;
    friend QDebug operator<<(QDebug out,const XmlPart &c);
};

QDebug operator<<(QDebug out,const XmlPart &c)
{
    QDEBUG_CLASS_START(out,XmlPart,c)
    QDEBUG_CLASS_MEMBER(name)
    QDEBUG_CLASS_LIST(XmlFiles,fileList)
    QDEBUG_CLASS_END()
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
    QStringList dependencies; 
    QMap<QString,XmlPart*> partList;
    friend QDebug operator<<(QDebug out,const XmlPackage &c);
};

QDebug operator<<(QDebug out,const XmlPackage &c)
{
    QDEBUG_CLASS_START(out,XmlPackage,c)
    QDEBUG_CLASS_MEMBER(name)
    QDEBUG_CLASS_MEMBER(description)
    QDEBUG_CLASS_MEMBER(dependencies)
    QDEBUG_CLASS_LIST(XmlPart,partList)
    QDEBUG_CLASS_END()
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
    QDEBUG_CLASS_START(out,XmlModule,c)
    QDEBUG_CLASS_MEMBER(name)
    QDEBUG_CLASS_LIST(XmlPackage,packageList)
    QDEBUG_CLASS_END()
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
    QDEBUG_CLASS_START(out,XmlData,c)
    QDEBUG_CLASS_LIST(XmlModule,moduleList)
    QDEBUG_CLASS_END()
}

class MyXmlHandler : public QXmlDefaultHandler
{
public:
    MyXmlHandler(XmlData *data) : m_data(data), m_level(0)
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
            m_level++;
        }
        else if (qName == "package")
        {
            m_parent = m_last;
            m_package = new XmlPackage(atts);
            m_module->packageList[atts.value("name")] = m_package;
            m_level++;
        }
        else if (qName == "part")
        {
            m_parent = m_last;
            m_part = new XmlPart(atts);
            m_package->partList[atts.value("name")] = m_part;
            m_level++;
        }
        else if (qName == "files")
        {
#if 0 
            if (m_level == 1)
                qDebug() << "module level";
#endif
            m_parent = m_last;
            m_files = new XmlFiles(atts);
            m_part->fileList.append(m_files);
            m_level++;
            // add case for files outside of part 
        }

        m_last = qName;
        return true;
    }
    
    bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName )
    {
        inElement = false;
        m_level--;
        return true;
    }

    bool characters ( const QString & ch )  
    {
        if  (!inElement)    
            return true;
        // handle in element data
        if (element == "shortDescription")
            m_package->description = ch;
        else if (element == "dependency")
        {
            if (!m_package->dependencies.contains(ch))
                m_package->dependencies.append(ch.toLower());
        }
        else if (element == "files" & !ch.isEmpty())
        {
            // ch contains content for tag <files>file; file </files>
            foreach(const QString &file, ch.split(ch.contains(';') ? ';': '\x0a',QString::SkipEmptyParts))
            {
                if (!file.trimmed().isEmpty())
                    m_files->fileList.append(file.trimmed());
            }
        } 
            
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
        XmlFiles *m_files;
        XmlData *m_data;
        int m_level;
};

bool findFiles(QList<InstallFile> &fileList, const QString& aDir, const QString &root)
{
    QDir dir( aDir );
    if (dir.exists())//QDir::NoDotAndDotDot
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | 
        QDir::Dirs | QDir::Files);
        int count = entries.size();
        foreach(QFileInfo entryInfo, entries)
        {
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                findFiles(fileList,path,root);
            }
            else
            {
                InstallFile aFile(path.remove(root));
                fileList.append(aFile);
            }
        }
    }
    return true;
}

XmlTemplatePackager::XmlTemplatePackager(const QString &packageName, const QString &packageVersion,const QString &notes)
    : Packager(packageName, packageVersion, notes)
{
    m_data = new XmlData;
    m_debug = !qgetenv("DEBUG").isEmpty();
}

XmlTemplatePackager::~XmlTemplatePackager()
{
    delete m_data;
}

bool XmlTemplatePackager::parseConfig(const QString &fileName)
{
    QXmlDefaultHandler *handler = new MyXmlHandler(m_data);    
    QXmlSimpleReader xmlReader;
    QFile file(fileName);
    xmlReader.setContentHandler(handler);
    xmlReader.setErrorHandler(handler);

    QXmlInputSource *source = new QXmlInputSource(&file);
    bool ok = xmlReader.parse(source);
    if (m_debug)
        qDebug() << "parsed xml schema\n" << *m_data;
    return ok;
}

bool XmlTemplatePackager::makePackage(const QString &dir, const QString &destdir, bool bComplete)
{
    QFileInfo fi(dir);
    findFiles(m_fileList, dir, fi.absoluteFilePath()+ (!dir.endsWith('/') ? "/" : "") );

    if (m_debug)
        qDebug() << "rootdir:" << dir << m_fileList;

    QStringList modules = m_data->moduleList.keys();
    if (modules.size() == 0)
        return false;

    // get fist module entry regaredless of name
    XmlModule *m = m_data->moduleList[modules[0]];
    m_currentModel = m;

    // iterate through all defined packages
    foreach(const QString &key, m->packageList.keys())
    {
        XmlPackage *p = m->packageList[key];
        if (key != "default")
        {
            m_name = p->name;
            m_notes = p->description;
            m_dependencies = p->dependencies;
        }
        m_currentPackage = p;
        if (!Packager::makePackage(dir,destdir,bComplete))
            return false;
    }

    // print a list of unused files 
    qOut() << "----------- unused files -----------\n";
    foreach(const InstallFile &file, m_fileList)
    {
        if (!file.usedFile)
            qOut() << file.inputFile << "\n";
    }

    return true;
}
    
bool XmlTemplatePackager::generatePackageFileList(QList<InstallFile> &fileList, Packager::Type type, const QString &root)
{
    QString dir = root.isEmpty() ? m_rootDir : root;

    QString compilerType; 
    if (m_type == "mingw")
        compilerType = "mingw";
    else if (m_type == "mingw4")
        compilerType = "mingw4";
    else if (m_type == "vc90")
        compilerType = "vc90";
    else if (m_type == "vc100")
        compilerType = "vc100";
    else 
        compilerType = "vc80";
    
    QString packageType;
    if (type == Packager::BIN)
        packageType = "runtime";
    else if (type == Packager::LIB)
        packageType = "development";
    else if (type == Packager::DOC)
        packageType = "documentation";
    else if (type == Packager::SRC)
        packageType = "source";
    else if (type == Packager::DBG)
        packageType = "symbols";
    else if (type == Packager::ALL)
        packageType = "all";
            
    fileList.clear();

    // try package shortcuts 
    XmlPart *part = m_currentPackage->partList[packageType];
    if (!part)
    {
        if (type == Packager::BIN)
            packageType = "bin";
        else if (type == Packager::LIB)
            packageType = "lib";
        else if (type == Packager::DOC)
            packageType = "doc";
        else if (type == Packager::SRC)
            packageType = "src";
        else if (type == Packager::DBG)
            packageType = "dbg";
        else if (type == Packager::ALL)
            packageType = "all";
        part = m_currentPackage->partList[packageType];
        if (!part)
            return false;
    }    
        
    if (part->fileList.size() > 0)
    {
        foreach(XmlFiles *f, part->fileList)
        {
            if (!f->compiler.isEmpty() && compilerType != f->compiler)
            {
                if (m_debug)
                    qDebug() << *f << "ignored";                    
                continue;
            }
            if (m_debug)
                qDebug() << *f << "added"; 

            // new style regex 
            if (!f->include.isEmpty() && f->directory.isEmpty())
            {
                if (m_debug)
                    qOut() << f->include << "----------------------";
                QRegExp rx(f->include);
                for (QList<InstallFile>::iterator i = m_fileList.begin(); i != m_fileList.end(); ++i)
                {
                    InstallFile &file = *i;
                    int pos = 0;
                    if (rx.indexIn(file.inputFile,pos) != -1)
                    {
                        if (file.usedFile) {
                            qError() << "while creating " << m_currentPackage->name << " part " << part->name  << " file " <<  file.inputFile <<" ignored, because it was already used\n";
                            continue;
                        }
                        fileList.append(file);
                        file.usedFile = true;
                    }
                }            
            }
            else if (f->handler == "parseQtIncludeFiles")
                parseQtIncludeFiles(fileList, dir, f->directory,  f->include, f->exclude);
            else if (!f->directory.isEmpty())
                generateFileList(fileList, dir, f->directory,  f->include, f->exclude);
            else 
            {
                for (QList<InstallFile>::iterator i = m_fileList.begin(); i != m_fileList.end(); ++i)
                {
                    InstallFile &file = *i;
                    if (f->fileList.contains(file.inputFile))
                    {
                        if (file.usedFile) {
                            qError() << "while creating " << m_currentPackage->name << " part " << part->name  << " file " <<  file.inputFile <<" ignored, because it was already used\n";
                            continue;
                        }
                        file.usedFile = true;
                        fileList.append(file);
                    }
                }
            }
        }
    }

#if 0    
    // create the meta package
    if(type == Packager::BIN && m_currentModel->name == m_currentPackage->name
        && fileList.size() == 0)
    {
        qDebug() << "create the meta package";
        QFileInfo fi(dir + "/share/doc/"+m_name+"/readme.txt");
        QDir d;
        d.mkpath(fi.absolutePath());
        QFile f(fi.absoluteFilePath());
        if(f.open(QIODevice::WriteOnly))
        {
            f.write(QByteArray("This package is a meta package\n"));
            f.close();
            fileList.append(InstallFile(m_name));
        }
    }
#endif
    
    if (m_debug) 
        qOut() << "generated filelist\n" << fileList;
    
    return true;
}

