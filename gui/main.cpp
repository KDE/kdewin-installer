/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>
#include <QPlastiqueStyle>

#include "installwizard.h"
#include "misc.h"

// FIXME: required by uninstaller progress
InstallWizard *wizard;


int main(int argc, char *argv[])
{
    setMessageHandler();
    QApplication app(argc, argv);

#if 0
    // enable to have a bit more KDE look
    QApplication::setStyle(new QPlastiqueStyle);
    //QApplication::setPalette(QApplication::style()->standardPalette());
#endif

    wizard = new InstallWizard();
    return wizard->exec();
}
