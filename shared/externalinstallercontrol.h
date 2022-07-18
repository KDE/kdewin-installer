/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef EXTERNALINSTALLERCONTROL_H
#define EXTERNALINSTALLERCONTROL_H

#include <QtDebug>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

class InstallerControlType {
    public:
        bool parse(const QString &line); 
        bool parse(const QStringList &line); 
        QString type() { return m_type; }
        void setType(const QString &type) { m_type = type; }
        QString caption() { return m_caption; }
        void setCaption(const QString &caption) { m_caption = caption; }
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
#ifdef Q_OS_WIN32
        bool connect(HANDLE handle);
#endif
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
#ifdef Q_OS_WIN32
        DWORD m_processId;
#endif
};

#endif
