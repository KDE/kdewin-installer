/****************************************************************************
**
** Copyright (C) 2005-2008  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtDebug>
#include <QCoreApplication>

#include "package.h"

int main(int argc, char ** argv)
{
    QCoreApplication app(argc,argv);

    Package::PackageVersion a("1.0.5");
    Package::PackageVersion b("1.0.4");
    Package::PackageVersion c("1.00.3");
    Package::PackageVersion d(b);

	Q_ASSERT(a > b);
// fix this
#if 1
	Q_ASSERT(c > a);
	Q_ASSERT(c > b);
	Q_ASSERT(a < c);
	Q_ASSERT(b < c);
#else
	Q_ASSERT(a > c);
	Q_ASSERT(b > c);
	Q_ASSERT(c < b);
	Q_ASSERT(c < c);
#endif
	Q_ASSERT(d == b);
	Q_ASSERT(a != b);
					

    return 0;
}

