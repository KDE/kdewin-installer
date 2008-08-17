/* This file is part of the KDE Project

   Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>
   All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef _LINKFILE_H
#define _LINKFILE_H

#include <QList>
#include <QString>
#include <QStringList>

class LinkFile {
    public:
        /// create instance 
        LinkFile(const QString &_linkPath, const QString &_execPath, const QString &_description, const QString &_workingDir)
        {
            m_execPath     = _execPath;    
            m_arguments    = QStringList();
            m_linkPath     = _linkPath;
            m_description  = _description; 
            m_workingDir   = _workingDir;  
        }
        /** 
            constructs  LinkFile instance with arguments
                    */
        LinkFile(const QString &linkPath, const QStringList &args, const QString &description, const QString &workingDir)
        {
            if (args.size() > 0)
                m_execPath = args[0];
            if (args.size() > 1) {
                m_arguments = args;
                m_arguments.removeFirst();
            }
            m_linkPath     = linkPath;
            m_description  = description; 
            m_workingDir   = workingDir;  
        }
        
        /// check if link file exists
        bool exists();
        /// create link file from instance data 
        bool create();
        /// remove link file
        bool remove();
        /// read link file content into instance 
        bool read();

        const QString &execPath()    { return m_execPath; }    
        const QString &linkPath()    { return m_linkPath; }
        const QString &description() { return m_description; }
        const QString &workingDir()  { return m_workingDir; }  
        const QStringList &arguments()   { return m_arguments; }  

        void setExecPath(const QString &a)    { m_execPath = a; }    
        void setLinkPath(const QString &a)    { m_linkPath = a; }
        void setDescription(const QString &a) { m_description = a; }
        void setWorkingDir(const QString &a)  { m_workingDir = a; }  
        void setArguments(const QStringList &a)   { m_arguments = a; }  

        friend QDebug operator<<(QDebug out, const LinkFile &c);

    protected:
        QString m_execPath;    
        QString m_linkPath;
        QString m_description; 
        QString m_workingDir;  
        QStringList m_arguments;    
};

class LinkFiles {
    public:
        static bool scan(QList <LinkFile> &files, const QString &rootDir);
        static bool create(QList <LinkFile> &newFiles);
        static bool cleanup(QList <LinkFile> &newFiles, QList <LinkFile> &oldFiles);
};

#endif
// vim: ts=4 sw=4 et
