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

#define DATA_DIR "tests/data/misc"

void TestMisc::testGenerateFileList()
{
    QList<InstallFile> files;
    QString root(CMAKE_SOURCE_DIR);
    QString subdir(DATA_DIR);
    QString filter("*.txt");
    QCOMPARE(generateFileList(files, root, subdir, filter), true);
    QCOMPARE(files.size(), 1);
    QCOMPARE(files[0].inputFile, QString(DATA_DIR "/config-site.txt"));
    QCOMPARE(files[0].outputFile, QString());
}

void TestMisc::testGenerateFileListManifest()
{
    QList<InstallFile> files;
    QString root(CMAKE_SOURCE_DIR);
    QString subdir(DATA_DIR);
    QString filter("*");
    QString exclude("*.txt");
    QCOMPARE(generateFileList(files, root, subdir, filter, exclude), true);
    QCOMPARE(files.size(), 5);
    qDebug() << files;
    QCOMPARE(files[0].inputFile, QString(DATA_DIR "/test.dll"));
    QCOMPARE(files[0].outputFile, QString());
    QCOMPARE(files[1].inputFile, QString(DATA_DIR "/test.exe"));
    QCOMPARE(files[1].outputFile, QString());
    QCOMPARE(files[2].inputFile, QString(DATA_DIR "/without-manifest.exe"));
    QCOMPARE(files[2].outputFile, QString());
    QCOMPARE(files[3].inputFile, QString(DATA_DIR "/test.dll.manifest"));
    QCOMPARE(files[3].outputFile, QString());
    QCOMPARE(files[4].inputFile, QString(DATA_DIR "/test.exe.manifest"));
    QCOMPARE(files[4].outputFile, QString());
}

void TestMisc::testGenerateFileListRegExpExcludes()
{
    QList<InstallFile> files;
    QString root(CMAKE_SOURCE_DIR);
    QString subdir(DATA_DIR);
    QString filter("*.txt");
    QList<QRegExp> excludes;
    QRegExp rx("config-site.*");
    rx.setPatternSyntax(QRegExp::Wildcard);
    excludes << rx;
    QCOMPARE(generateFileList(files, root, subdir, filter, excludes), true);
    QCOMPARE(files.size(), 0);
}

QTEST_MAIN(TestMisc)
