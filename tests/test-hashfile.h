/****************************************************************************
**
** Copyright (C) 2023 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef TEST_HASHFILE_H

#include <QObject>
#include <QTest>

class TestHashFile: public QObject
{
    Q_OBJECT
private slots:
    void test1();
};

#endif
