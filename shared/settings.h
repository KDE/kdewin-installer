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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

/**
 holds global options, which are stored in a local user specific config file
*/

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings();

    // Place where the packages should be installed to
    QString installDir();
    void setInstallDir(const QString &dir);

    // Place where the packages should be downloaded to
    QString downloadDir();
    void setDownloadDir(const QString &dir);

    // show title page
    bool showTitlePage();
    void setShowTitlePage(bool bShow);

    // create start menu entries fomr .desktop files
    bool createStartMenuEntries();
    void setCreateStartMenuEntries(bool bCreate);

    // true on first run
    bool isFirstRun();

    static Settings &getInstance();
    
Q_SIGNALS:
    void installDirChanged(const QString &newDir);
    void downloadDirChanged(const QString &newDir);
protected:
private:
    QSettings m_settings;
};



#endif
