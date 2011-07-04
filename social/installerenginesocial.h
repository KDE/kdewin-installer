#ifndef INSTALLERENGINESOCIAL_H
#define INSTALLERENGINESOCIAL_H
#include"installerengine.h"
#include"installerprogress.h"
#include <QProcess>
class InstallerEngineSocial : public InstallerEngine
{
    Q_OBJECT
public:
    explicit InstallerEngineSocial(QObject *parent = 0);
    QStringList getDependencies(QString package_name);
    bool installpackage(QString name);
signals:
    void packageInstalled(QString);
    void packagesToInstall(int);
    void postInstalationStart();
    void postInstalationEnd();
public slots:
private:
    bool postInstallTasks();

};

#endif // INSTALLERENGINESOCIAL_H
