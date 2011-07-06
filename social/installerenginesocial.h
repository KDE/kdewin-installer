#ifndef INSTALLERENGINESOCIAL_H
#define INSTALLERENGINESOCIAL_H
#include"installerengine.h"
#include"installerprogresssocial.h"
#include <QProcess>
class InstallerEngineSocial : public InstallerEngine
{
    Q_OBJECT
public:
    explicit InstallerEngineSocial(QObject *parent = 0);
    QStringList getDependencies(QString package_name);
signals:
    void packageInstalled(QString);
    void packagesToInstall(int);
    void postInstalationStart();
    void postInstalationEnd();
public slots:
    bool installpackage(QString name);
private:
    bool postInstallTasks();
protected:
    bool runCommand(const QString &msg, const QString &app, const QStringList &params=QStringList());

};

#endif // INSTALLERENGINESOCIAL_H
