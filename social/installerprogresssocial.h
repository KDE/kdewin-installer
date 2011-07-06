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

#ifndef INSTALLERPROGRESS_H
#define INSTALLERPROGRESS_H

#include <QWidget>

namespace Ui {
    class installerprogress;
}

class InstallerProgressSocial : public QWidget
{
    Q_OBJECT

public:
    explicit InstallerProgressSocial(QWidget *parent = 0);
    ~InstallerProgressSocial();
public slots:
    void getpackageno(int n) { packages = n; this->show(); installedpackages=-1;  }
    void packageinstalled(QString name) { ++installedpackages; update_progressbar(name); }
    void FinishedInstallMenuItems();
    void InstallMenuItems();
private:
    Ui::installerprogress *ui;
    int packages;
    int installedpackages;
    void update_progressbar(QString);
};

#endif // INSTALLERPROGRESS_H
