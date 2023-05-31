/****************************************************************************
**
** Copyright (C) 2023 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "test-misc.h"

#include "config.h"
#include "misc.h"

#include <QTest>

void TestMisc::testGenerateFileList()
{
    QList<InstallFile> files;
    QString root(CMAKE_SOURCE_DIR);
    QString subdir("tests/data");
    QString filter("*.txt");
    QCOMPARE(generateFileList(files, root, subdir, filter), true);
    QCOMPARE(files.size(), 3);
    QCOMPARE(files[0].inputFile, "tests/data/config-site.txt");
    QCOMPARE(files[1].inputFile, "tests/data/test-hashfile space.txt");
    QCOMPARE(files[2].inputFile, "tests/data/test-hashfile.txt");
}

void TestMisc::testGenerateFileListRegExpExcludes()
{
    QList<InstallFile> files;
    QString root(CMAKE_SOURCE_DIR);
    QString subdir("tests/data");
    QString filter("*.txt");
    QList<QRegExp> excludes;
    QRegExp rx("*config-site*");
    rx.setPatternSyntax(QRegExp::Wildcard);
    excludes << rx;
    QCOMPARE(generateFileList(files, root, subdir, filter, excludes), true);
    QCOMPARE(files.size(), 2);
    QCOMPARE(files[0].inputFile, "tests/data/test-hashfile space.txt");
    QCOMPARE(files[1].inputFile, "tests/data/test-hashfile.txt");
}

QTEST_MAIN(TestMisc)
