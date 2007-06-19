/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>

#include "installwizard.h"
#include "misc.h"

// FIXME: required by uninstaller progress
InstallWizard *wizard;


int main(int argc, char *argv[])
{
    setMessageHandler();
    QApplication app(argc, argv);

    wizard = new InstallWizard();
    return wizard->exec();
}
