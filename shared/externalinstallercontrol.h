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
#ifndef EXTERNALINSTALLERCONTROL_H
#define EXTERNALINSTALLERCONTROL_H

#include <QtDebug>
#include <windows.h>

class InstallerControlType {
    public:
        bool parse(const QString &line); 
        bool parse(const QStringList &line); 
        QString type() { return m_type; }
        QString setType(const QString &type) { m_type = type; }
        QString caption() { return m_caption; }
        QString setCaption(const QString &caption) { m_caption = caption; }
        QString timeout() { return m_timeout; }
 
    protected:
        QString m_type;    // window item className 
        QString m_caption; // window item titleName  
        QString m_timeout; 
};

typedef QList<InstallerControlType>  InstallerControlTypeList; 

class QProcess;
class ExternalInstallerControlPrivate;

/**    
   \brief The ExternalInstallerControl class provides support for controlling external installers by selecting buttons and set text in edit fields
   */
class ExternalInstallerControl {
    public:
        ExternalInstallerControl();
        ~ExternalInstallerControl();
        bool connect(int pid);
        bool connect(HANDLE handle);
        bool connect(const QProcess &proc);
        bool disconnect();
        
        /// press button whi
        bool pressButton(const QString &caption);
        bool fillInputField(const QString &caption, const QString &text);


        friend QDebug &operator<<(QDebug &,const ExternalInstallerControl &);
    protected:
        bool updateWindowItems();

    private:
        ExternalInstallerControlPrivate *d;
        DWORD m_processId;
};

#endif
