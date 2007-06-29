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

#ifdef ENABLE_STYLE
    QApplication::setStyle(new QPlastiqueStyle);
    QApplication::setPalette(QApplication::style()->standardPalette());

    qApp->setStyleSheet(
        "QDialog {"
            "background: #afafaf;"
            //"background: #2f2f2f;"
            "color: white"
        "}"
        "QLabel#topLabel {"
            "color: white;"
            "font-size: 20px;"
            "font-family: arial;"
            //"background-color: #92847b;"
            "background-image: url(:/images/brown.png);"
        "}"
        "QTreeView {"
            "background-image: url(:/images/gray.png);"
            "border: 2px solid gray;"
            "selection-background-color: #92847b;"
        "}"
        "QHeadView {"
            "background-color: #92847b;"
        "}"
        );
#endif

    wizard = new InstallWizard();
    return wizard->exec();
}
