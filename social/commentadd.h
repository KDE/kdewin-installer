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
