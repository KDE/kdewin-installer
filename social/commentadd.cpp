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

#include "commentadd.h"
#include "ui_commentadd.h"
#include <attica/providermanager.h>
#include <attica/provider.h>
#include <attica/comment.h>
#include <attica/itemjob.h>
#include <attica/postjob.h>

#include <QMessageBox>
#include "softwaredetails.h"

CommentAdd::CommentAdd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommentAdd)
{
    ui->setupUi(this);

    connect(ui->send, SIGNAL(clicked()), this, SLOT(sendComment()));
}

CommentAdd::~CommentAdd()
{
    delete ui;
}

void CommentAdd::sendComment()
{
    m_provider.saveCredentials(ui->name->text(), QString());
    Attica::ItemPostJob < Attica::Comment >*job =  m_provider.addNewComment(Attica::Comment::ContentComment,m_content_id, 0, 0, ui->subject->text(),ui->message->toPlainText());
    connect(job, SIGNAL(finished(Attica::BaseJob *)), this, SLOT(commentDelivered(Attica::BaseJob *)));
    job->start();
}
void CommentAdd::setProvider(Attica::Provider &provider, QString content_id)
{
    m_content_id = content_id;
    m_provider = provider;
}
void CommentAdd::commentDelivered(Attica::BaseJob * job)
{
    Attica::ItemPostJob < Attica::Comment> * comment = static_cast < Attica::ItemPostJob <Attica::Comment> * > (job);
    if (comment->metadata().error() == Attica::Metadata::NoError)
    {
        ui->message->setText(QString(""));
        ui->subject->setText(QString(""));
        emit commentAdded();
    }
    else
        QMessageBox::warning(this, QString("Error"),"Comment could not be added",QMessageBox::Ok);
}
