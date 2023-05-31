/****************************************************************************
**
** Copyright (C) 2023 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef TEST_MISC_H
#define TEST_MISC_H

#include <QObject>

class TestMisc : public QObject
{
    Q_OBJECT
private slots:
    void testGenerateFileList();
    void testGenerateFileListRegExpExcludes();
};

#endif // TEST_MISC_H
