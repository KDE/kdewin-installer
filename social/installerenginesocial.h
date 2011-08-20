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

#ifndef INSTALLERENGINESOCIAL_H
#define INSTALLERENGINESOCIAL_H
#include"installerengine.h"
#include"installerprogresssocial.h"
#include <QProcess>
class InstallerEngineSocial : public InstallerEngine
{
    Q_OBJECT
public:
    explicit InstallerEngineSocial(QObject *parent = 0);
    QStringList getDependencies(QString package_name);
    bool isPackageInstalled(QString name);
signals:
    void packageInstalled(QString);
    void packagesToInstall(int);
    void postInstalationStart();
    void postInstalationEnd();
public slots:
    bool installpackage(QString name);
    bool uninstallpackage(QString name);
    void changeRoot(QString);
private:
    bool postInstallTasks();
protected:
    bool runCommand(const QString &msg, const QString &app, const QStringList &params=QStringList());

};


#endif // INSTALLERENGINESOCIAL_H
