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

#ifndef SOFTWAREDETAILS_H
#define SOFTWAREDETAILS_H

#include <QWidget>
#include <attica/content.h>
#include <attica/provider.h>
#include <QVBoxLayout>
#include <QScrollArea>

namespace Ui {
    class SoftwareDetails;
}

class SoftwareDetails : public QWidget
{
    Q_OBJECT


public:
    explicit SoftwareDetails(QWidget *parent = 0);
    ~SoftwareDetails();
    void setContent(Attica::Content * content);
    void setProvider(Attica::Provider  &provider);
    Attica::Content *m_content;
    void install_status_changed();
private:
    Ui::SoftwareDetails *ui;

    Attica::Provider m_provider;
    QWidget * comments;
    QScrollArea *comments_scroll;
signals:
    void installpackage(QString);
    void uninstallpackage(QString);
    void commentsChanged();
private slots:
    void installbuttonclicked();
    void showComments();
    void updateComments();
    void commentsLoaded(Attica::BaseJob *);

  //  void resizelogo();

};

#endif // SOFTWAREDETAILS_H
