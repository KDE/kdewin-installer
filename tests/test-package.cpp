/****************************************************************************
**
** Copyright (C) 2005-2008  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
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

