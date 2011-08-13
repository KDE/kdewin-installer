#ifndef COMMENT_H
#define COMMENT_H

#include <QWidget>

namespace Ui {
    class Comment;
}

class Comment : public QWidget
{
    Q_OBJECT

public:
    explicit Comment(QWidget *parent = 0);
    ~Comment();
    void setData(QString nickname, QString subject, QString message);
private:
    Ui::Comment *ui;
};

#endif // COMMENT_H
