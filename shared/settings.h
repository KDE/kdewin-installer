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
	bool isFirstRun() { return m_settings.value("FirstRun", true).toBool(); }
	void setFirstRun(bool bFirstRun) { m_settings.setValue("FirstRun", bFirstRun); sync(); }

	// use nested download directory tree 
	bool nestedDownloadTree() { return m_settings.value("nestedDownloadTree", false).toBool(); }
	void setNestedDownloadTree(bool value) { m_settings.setValue("nestedDownloadTree", value); sync(); }

	int proxyMode() { return m_settings.value("proxyMode",0).toInt(); }
	const QString proxyHost() { return m_settings.value("proxyHost").toString(); }
	int proxyPort() { return m_settings.value("proxyPort").toInt(); }
	void setProxy(bool off, bool ie, bool manual, const QString &host, const QString &port) 
	{
		if (off) m_settings.setValue("proxyMode",0);
		if (ie) m_settings.setValue("proxyMode",1);
		if (manual) m_settings.setValue("proxyMode",2);
		m_settings.setValue("proxyHost",host);
		m_settings.setValue("proxyPort",port);
		sync();
	}

    static Settings &getInstance();
    
    // QSettings compatible interface
    void beginGroup(const QString &prefix) { m_settings.beginGroup(prefix); }
    void endGroup() { m_settings.endGroup(); }
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const { return m_settings.value(key,defaultValue); }
    void setValue(const QString &key, const QVariant &value) { m_settings.setValue(key,value); }
	// sync database 
	void sync() { m_settings.sync(); }

Q_SIGNALS:
    void installDirChanged(const QString &newDir);
    void downloadDirChanged(const QString &newDir);
protected:
private:
    QSettings m_settings;
};



#endif
