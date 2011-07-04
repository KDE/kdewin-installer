#ifndef SOFTWAREDETAILS_H
#define SOFTWAREDETAILS_H

#include <QWidget>

namespace Ui {
    class SoftwareDetails;
}

class SoftwareDetails : public QWidget
{
    Q_OBJECT

public:
    explicit SoftwareDetails(QWidget *parent = 0);
    ~SoftwareDetails();

private:
    Ui::SoftwareDetails *ui;
};

#endif // SOFTWAREDETAILS_H
