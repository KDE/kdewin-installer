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

#include "softwaredetails.h"
#include "ui_softwaredetails.h"
#include <attica/content.h>
#include <attica/icon.h>
#include "debug.h"
#include "downloader.h"
#include "commentadd.h"
#include "comment.h"

SoftwareDetails::SoftwareDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoftwareDetails)
{
    ui->setupUi(this);

    connect(ui->install_button,SIGNAL(clicked()),this,SLOT(installbuttonclicked()));
    connect(ui->comments, SIGNAL(clicked()), this, SLOT(showComments()));
    connect(this, SIGNAL(commentsChanged()), this, SLOT(updateComments()));

}

SoftwareDetails::~SoftwareDetails()
{
    delete ui;
}
void SoftwareDetails::setContent(Attica::Content * content)
{
    QString gallery;
    int i = 1;
    QString pic_url;
    gallery.append("<div syle='white-space: pre'>");
    QString previewpic = QString("previewpic");
    while ((pic_url = content->attribute(QString("previewpic").append(QString::number(i))))!=QString())
        gallery.append(QString("<a href='%1'><img src='%1' height='100'></img></a>  ").arg(pic_url)), ++i;
    gallery.append("</div>");
    this->setWindowTitle(content->name());
    ui->textBrowser->setText(gallery.append(content->description()));
    ui->software_title->setText(content->name());
    m_content = content;
    if (!m_content->icons().empty())
    {
        QByteArray data;
        Attica::Icon icon = m_content->icons().first();
        Downloader::instance()->fetch(icon.url(), data);
        qDebug()<<"fetched icon";
        QGraphicsScene *scene = new QGraphicsScene();
        QPixmap pixmap;
        pixmap.loadFromData(data);
        scene->addPixmap(pixmap.scaled(ui->graphicsView->width(),ui->graphicsView->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation ));
       // scene->setSceneRect(QRect());
        ui->graphicsView->setScene(scene);
       // ui->graphicsView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        ui->graphicsView->show();

    }
    if (m_content->downloadUrlDescription(1).packageName() == QString())
        ui->install_button->setEnabled(false);

}

void SoftwareDetails::installbuttonclicked()
{
    ui->install_button->setEnabled(false);
    if (ui->install_button->text() == QString("Install"))
    {
        emit installpackage(m_content->downloadUrlDescription(1).packageName());
        qDebug()<<"install signal dispatched with packagename:"<<m_content->downloadUrlDescription(1).packageName();
    }
    else
    {
        emit uninstallpackage(m_content->downloadUrlDescription(1).packageName());
        qDebug()<<"uninstall signal dispatched with packagename:"<<m_content->downloadUrlDescription(1).packageName();
    }
    this->install_status_changed();
    ui->install_button->setEnabled(true);
}

void SoftwareDetails::setProvider(Attica::Provider &provider)
{
    m_provider = provider;
}

void SoftwareDetails::showComments()
{
    if (ui->horizontalLayout->count() > 1 && ui->horizontalLayout->itemAt(1)->widget()->isVisible())
    {
        ui->horizontalLayout->itemAt(1)->widget()->hide();
        QRect window_size = this->geometry();
        window_size.setWidth(window_size.width()-400);
        this->setGeometry(window_size);
       // ui->horizontalLayout->removeItem(ui->horizontalLayout->itemAt(1));
    }
    else
    {
        if (comments == NULL)
        {

            comments = new QWidget();
            comments->setLayout(new QVBoxLayout());
            CommentAdd * comment_add = new CommentAdd(this);
            comment_add->setProvider(m_provider, m_content->id());
            comment_add->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            comments->layout()->addWidget(comment_add);
            connect(comment_add, SIGNAL(commentAdded()), this, SLOT(updateComments()));

            comments_scroll = new QScrollArea();
            comments_scroll->setWidgetResizable(true);
            comments_scroll->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            comments_scroll->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            comments_scroll->setMinimumSize(400,0);
            comments_scroll->setWidget(new QWidget());
            comments_scroll->widget()->setLayout(new QVBoxLayout());
            comments_scroll->widget()->layout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            comments->layout()->addWidget(comments_scroll);
            ui->horizontalLayout->addWidget(comments);

            emit commentsChanged();

        }
        QRect window_size = this->geometry();
        window_size.setWidth(window_size.width()+400);
        this->setGeometry(window_size);
        comments->show();
    }
}
void SoftwareDetails::updateComments()
{
    Attica::ListJob < Attica::Comment> * job = m_provider.requestComments(Attica::Comment::ContentComment,m_content->id(),"0",0,20);
    connect(job, SIGNAL(finished(Attica::BaseJob*)), this, SLOT(commentsLoaded(Attica::BaseJob*)));
    job->start();

}

void SoftwareDetails::commentsLoaded(Attica::BaseJob * job)
{
    qDebug()<<"comments loaded";
    Attica::ListJob < Attica::Comment > *CommentList = static_cast < Attica::ListJob < Attica::Comment > * >(job);
    Attica::Comment::List l (CommentList->itemList());
    while(comments_scroll->widget()->layout()->count() > 0)
        comments_scroll->widget()->layout()->removeItem(comments_scroll->widget()->layout()->itemAt(0));
    for(Attica::Comment::List::Iterator it = l.begin(); it!=l.end(); ++ it)
    {

        Attica::Comment temp_comment(*it);
        Comment *tmp = new Comment();
        qDebug()<<"loading comment with subject"<<temp_comment.subject();
        //QWidget * test = comments_scroll->takeWidget();
        tmp->setData(temp_comment.user(), temp_comment.subject(), temp_comment.text());
        comments_scroll->widget()->layout()->addWidget(tmp);
        //comments_scroll->setWidget(test);
        //comments_scroll->show();
    }
}

void SoftwareDetails::install_status_changed()
{
    if (ui->install_button->text() == QString("Install"))
        ui->install_button->setText("Uninstall");
    else
        ui->install_button->setText("Install");
}
