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

#ifndef COMMENTADD_H
#define COMMENTADD_H

#include <QWidget>
#include <attica/provider.h>

namespace Ui {
    class CommentAdd;
}

class CommentAdd : public QWidget
{
    Q_OBJECT

public:
    explicit CommentAdd(QWidget *parent = 0);
    ~CommentAdd();
    void setProvider(Attica::Provider &provider, QString content_id);
signals:
    void commentAdded();

public slots:
    void commentDelivered(Attica::BaseJob * job);
private:
    Ui::CommentAdd *ui;
    Attica::Provider m_provider;
    QString m_content_id;
private slots:
    void sendComment();
};

#endif // COMMENTADD_H
