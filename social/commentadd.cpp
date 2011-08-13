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
