/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>

#include "installwizard.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    InstallWizard *wizard = new InstallWizard();
    return wizard->exec();
}
