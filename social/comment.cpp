#include "comment.h"
#include "ui_comment.h"

Comment::Comment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Comment)
{
    ui->setupUi(this);
}
void Comment::setData(QString nickname, QString subject, QString message)
{
    ui->nickname->setText(nickname);
    ui->message->setText(message);
}

Comment::~Comment()
{
    delete ui;
}
