#ifndef SOFTWAREDETAILS_H
#define SOFTWAREDETAILS_H

#include <QWidget>
#include <attica/content.h>

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

private:
    Ui::SoftwareDetails *ui;
    Attica::Content *m_content;
signals:
    void installpackage(QString);
private slots:
    void installbuttonclicked();
};

#endif // SOFTWAREDETAILS_H
