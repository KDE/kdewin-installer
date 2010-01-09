/******************************************************************************
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>. 
** All rights reserved.
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

#include "externalinstallercontrol.h"

#include <QCoreApplication>
#include <QProcess>

#ifdef Q_WS_WIN
#include <windows.h>
#else
#define Sleep sleep
#endif

bool InstallerControlType::parse(const QString &string)
{
    return parse(string.split(','));
}

bool InstallerControlType::parse(const QStringList &fields)
{
    if (fields.size() == 3)
    {
        m_type = fields[0];
        m_caption = fields[1];
        m_timeout = fields[2];
        return true;
    }

    else if (fields.size() == 2)
    {
        m_type = fields[0];
        m_caption = fields[1];
        return true;
    }
    else 
        return false;
}

class WindowItem {
    public:
#ifdef Q_WS_WIN
        HWND handle;
#endif
        QString className; 
        QString titleName;
        friend QDebug operator<< (QDebug out, const WindowItem &c);
};

typedef QList<WindowItem> WindowItemList;

class ExternalInstallerControlPrivate {
    public:
#ifdef Q_WS_WIN
        bool enumerateWindows(DWORD processID);
#endif
        
    private:
#ifdef Q_WS_WIN
        static BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam);
        static BOOL CALLBACK EnumChildWindowProc(HWND hwnd,LPARAM lParam);
        DWORD m_processId;
        HWND m_parentWindowsHandle;
#endif
        QList<WindowItem> items;
    friend class ExternalInstallerControl;
};

#ifdef Q_WS_WIN
BOOL CALLBACK ExternalInstallerControlPrivate::EnumChildWindowProc(HWND hwnd,LPARAM lParam)
{
    ExternalInstallerControlPrivate *p = (ExternalInstallerControlPrivate*)lParam;

    char className[MAX_PATH];
    char titleName[MAX_PATH];
    GetClassName(hwnd, className, MAX_PATH);
    GetWindowText(hwnd, titleName, MAX_PATH);    
    WindowItem item;
    item.handle = hwnd;
    item.className = className;
    item.titleName = titleName;
    p->items.append(item);
    return TRUE;
}

BOOL CALLBACK ExternalInstallerControlPrivate::EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
    ExternalInstallerControlPrivate *p = (ExternalInstallerControlPrivate*)lParam;

    DWORD dwProcessId;
    DWORD processID = GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId != p->m_processId)
        return TRUE;
    

    char className[MAX_PATH];
    char titleName[MAX_PATH];
    GetClassName(hwnd, className, MAX_PATH);
    GetWindowText(hwnd, titleName, MAX_PATH);    
    WindowItem item;
    item.handle = hwnd;
    item.className = className;
    item.titleName = titleName;
    p->items.append(item);
    p->m_parentWindowsHandle = hwnd;

    BOOL ret = EnumChildWindows(hwnd, ExternalInstallerControlPrivate::EnumChildWindowProc,lParam);
    return TRUE;
}

bool ExternalInstallerControlPrivate::enumerateWindows(DWORD processId)
{
    m_processId = processId;
    BOOL ret = EnumWindows(ExternalInstallerControlPrivate::EnumWindowsProc,(LPARAM)this);
    return true;
}
#endif

QDebug operator<< (QDebug out, const WindowItem &c)
{
    out << "WindowItem (" 
#ifdef Q_WS_WIN
        << "handle: " << c.handle
#endif
        << "className: " << c.className
        << "titleName: " << c.titleName
        << ")";
    return out;
}

QDebug operator<< (QDebug out, const WindowItemList &c)
{
    out << "WindowItemList (";
    Q_FOREACH(const WindowItem &item, c)
        out << item;
    out << ")";
    return out;
}

#ifdef Q_WS_WIN
// All credit to Nish
void SetForegroundWindowEx( HWND hWnd )
{
   //Attach foreground window thread to our thread
   const DWORD ForeGroundID = GetWindowThreadProcessId(::GetForegroundWindow(),NULL);
   const DWORD CurrentID   = GetCurrentThreadId();
 
   AttachThreadInput ( ForeGroundID, CurrentID, TRUE );
   //Do our stuff here
   HWND hLastActivePopupWnd = GetLastActivePopup( hWnd );
   SetActiveWindow( hLastActivePopupWnd );
 
   //Detach the attached thread
   AttachThreadInput ( ForeGroundID, CurrentID, FALSE );
}// End SetForegroundWindowEx

/// clear input text field 
void clearItemText(HWND hwnd)
{
    for (int i = 0; i < 50 ; i++)
    {
        PostMessage(hwnd,WM_KEYDOWN, VkKeyScan((char)8) ,0);
    }
}

/// set message into input text field 
void SetItemText(HWND hwnd, const QString &message)
{
    clearItemText(hwnd);
    for (int i = 0; i < message.size(); i++)
    {
        char c = message.at(i).toAscii();
        uint aKey=MapVirtualKey((uint)c,1);
        PostMessage(hwnd,WM_KEYDOWN,VkKeyScan(c) ,0);
        PostMessage(hwnd,WM_KEYUP,VkKeyScan(c) ,0);
    }
}
#endif

ExternalInstallerControl::ExternalInstallerControl()
{
    d = new ExternalInstallerControlPrivate;
}

ExternalInstallerControl::~ExternalInstallerControl()
{
    delete d;
}

/*
 bool ExternalInstallerControl::connect(const QString &windowTitle)
{
    BOOL ret = EnumWindows(EnumWindowsProc,(LPARAM)this);
    //HANDLE h = FindWindowEx(null, windowTitle.toLatin1().data());
}
*/

bool ExternalInstallerControl::connect(const QProcess &proc)
{
#ifdef Q_WS_WIN
    _PROCESS_INFORMATION* p = proc.pid();
    m_processId = p->dwProcessId;
#else
    // FIXME: there should be a Linux specific section here
#endif
    updateWindowItems();
    return d->items.size() > 0;
}

bool ExternalInstallerControl::updateWindowItems()
{
    d->items.clear();
    while(1) 
    {
#ifdef Q_WS_WIN
        bool ret = d->enumerateWindows(m_processId);
#else
        break;
#endif
        if (d->items.size() > 0)
            break;
        Sleep(1000);
    }
    qDebug() << d->items;
    return true;
}

bool ExternalInstallerControl::pressButton(const QString &caption)
{
    int interval = 1; // sec
    int timeout = 0;
    while (timeout < 60) // sec 
    {
        updateWindowItems();

        Q_FOREACH(const WindowItem &item, d->items)
        {
            if (item.className == "Button" && item.titleName == caption)
            {
                // setting breakpoints in the critical section below breaks 
                // the wanted action for unknown reason
                // any hints how to solve this problem are welcome

#ifdef Q_WS_WIN
                // --- start critial section --
                SetForegroundWindowEx(d->m_parentWindowsHandle);
                PostMessage(item.handle,WM_LBUTTONDOWN,0,0);
                PostMessage(item.handle,WM_LBUTTONUP,0,0);
#endif
                return true;
                // --- end critial section -- 
            }
        }
        Sleep(interval*1000);
        timeout++;
    }    
    return false;
}

bool ExternalInstallerControl::fillInputField(const QString &identifier, const QString &text)
{
    int interval = 1; // sec
    int timeout = 0;
    while (timeout < 60) // sec 
    {
        updateWindowItems();

        Q_FOREACH(const WindowItem &item, d->items)
        {
            if (item.className == "Button" && item.titleName == text)
            {
#ifdef Q_WS_WIN
                PostMessage(item.handle,WM_SETFOCUS,0,0);
                Sleep(1);
                PostMessage(item.handle,WM_LBUTTONUP,0,0);
                Sleep(1);
#endif
                return true;
            }
        }
        Sleep(interval*1000);
        timeout++;
    }    
    return false;
}

QDebug &operator<<(QDebug &out, const ExternalInstallerControl &c)
{
    out << "ExternalInstallerControl ("
        << c.d
        << ")";
    return out;
}
