/****************************************************************************
**
** Copyright (C) 2023 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "test-hashfile.h"

#include "config.h"
#include "hashfile.h"
#include "misc.h"

void TestHashFile::test1()
{
    HashFile hashFile(Hash::SHA1, "test-hashfile.txt", CMAKE_SOURCE_DIR "/tests/data/hashfile");
    QCOMPARE(hashFile.computeHash(), true);
    QCOMPARE(QString(hashFile.toHashFileContent()), QString("e9506d89dd4b301b77ebd059df5faffea732b921  test-hashfile.txt\n"));

    HashFile hashFile2(Hash::SHA1, CMAKE_SOURCE_DIR "/tests/data/hashfile/test-hashfile.txt");
    QCOMPARE(hashFile2.computeHash(), true);
    QCOMPARE(QString(hashFile2.toHashFileContent()), QString("e9506d89dd4b301b77ebd059df5faffea732b921  " CMAKE_SOURCE_DIR "/tests/data/hashfile/test-hashfile.txt\n"));

    HashFile hashFile3(Hash::SHA1, CMAKE_SOURCE_DIR "/tests/data/hashfile/test-hashfile space.txt");
    QCOMPARE(hashFile3.computeHash(), true);
    QCOMPARE(QString(hashFile3.toHashFileContent()), QString("e9506d89dd4b301b77ebd059df5faffea732b921  " CMAKE_SOURCE_DIR "/tests/data/hashfile/test-hashfile\\ space.txt\n"));

    InstallFile installFile(CMAKE_SOURCE_DIR "/tests/data/hashfile/test-hashfile.txt", "tests/test-hashfile.txt");
    HashFile hashFile4(Hash::SHA1, installFile);
    QCOMPARE(hashFile4.computeHash(), true);
    QCOMPARE(QString(hashFile4.toHashFileContent()), QString("e9506d89dd4b301b77ebd059df5faffea732b921  tests/test-hashfile.txt\n"));
}

QTEST_MAIN(TestHashFile)
