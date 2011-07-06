#ifndef INSTALLERPROGRESS_H
#define INSTALLERPROGRESS_H

#include <QWidget>

namespace Ui {
    class installerprogress;
}

class installerprogress : public QWidget
{
    Q_OBJECT

public:
    explicit installerprogress(QWidget *parent = 0);
    ~installerprogress();
public slots:
    void getpackageno(int n) { packages = n; this->show(); installedpackages=-1; }
    void packageinstalled(QString name) { ++installedpackages; update_progressbar(name); }
    void FinishedInstallMenuItems();
    void InstallMenuItems();
private:
    Ui::installerprogress *ui;
    int packages;
    int installedpackages;
    void update_progressbar(QString);
};

#endif // INSTALLERPROGRESS_H