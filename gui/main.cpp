/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>

#include "installwizard.h"


InstallWizard *wizard;


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

  InstallWizard *wizard = new InstallWizard();
  return wizard->exec();
}
