/****************************************************************************
**
** Copyright (C) 2011 Constantin Tudorica <tudalex@gmail.com>
** All rights reserved.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <attica/provider.h>
#include <attica/providermanager.h>
#include <QListWidget>
#include "installerprogress.h"
#include "installerenginesocial.h"
#include "settingspage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
    void providersChanged();
    void onContentRecieved(Attica::BaseJob*);
    void category_selected();
    void onContentListRecieved(Attica::BaseJob*);
    void softwareSelected(QListWidgetItem*);
    void displaySettings();
private:

    Attica::ProviderManager m_manager;
    QListWidget * m_SoftwareList;
    QVBoxLayout *m_categories;
    InstallerEngineSocial *m_installengine;
    Attica::Provider m_provider;


};

#endif // MAINWINDOW_H
