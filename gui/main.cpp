/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>
#include <QFile>

#include "installwizard.h"

// FIXME: required by uninstaller progress
InstallWizard *wizard;

QFile *log; 

void myMessageOutput(QtMsgType type, const char *msg)
 {
    switch (type) {
     case QtDebugMsg:
         log->write("Debug:");
         log->write(msg);
         log->write("\n");
         break;
     case QtWarningMsg:
         log->write("Warning:");
         log->write(msg);
         log->write("\n");
         break;
     case QtCriticalMsg:
         log->write("Critical:");
         log->write(msg);
         log->write("\n");
         break;
     case QtFatalMsg:
         log->write("Fatal:");
         log->write(msg);
         log->write("\n");
         abort();
     }
 }

int main(int argc, char *argv[])
{
    QFile f("kdewin-installer.log");
    log = &f;
    f.open(QIODevice::WriteOnly);

    qInstallMsgHandler(myMessageOutput);
    QApplication app(argc, argv);

    wizard = new InstallWizard();
    return wizard->exec();
}
