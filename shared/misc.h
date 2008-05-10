/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#ifndef MISC_H
#define MISC_H

#include <QtDebug>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QIODevice>

/*
 * Those three strings represent the Categories from freedesktop.org for .desktop files
 * http://standards.freedesktop.org/menu-spec/menu-spec-1.0.html
 * they are used to find the subcategories for startmenu entries
 */
#define FREEDESKTOP_MAIN_CATEGORIES "AudioVideo;Audio;Video;Development;Education;Game;Graphics;Network;Office;Settings;System;Utility;"
#define FREEDESKTOP_SUB_CATEGORIES "Building;Debugger;IDE;GUIDesigner;Profiling;RevisionControl;Translation;Calendar;ContactManagement;Database;Dictionary;Chart;Email;Finance;FlowChart;PDA;ProjectManagement;Presentation;Spreadsheet;WordProcessor;2DGraphics;VectorGraphics;RasterGraphics;3DGraphics;Scanning;OCR;Photography;Publishing;Viewer;TextTools;DesktopSettings;HardwareSettings;Printing;PackageManager;Dialup;InstantMessaging;Chat;IRCClient;FileTransfer;HamRadio;News;P2P;RemoteAccess;Telephony;TelephonyTools;VideoConference;WebBrowser;WebDevelopment;Midi;Mixer;Sequencer;Tuner;TV;AudioVideoEditing;Player;Recorder;DiscBurning;ActionGame;AdventureGame;ArcadeGame;BoardGame;BlocksGame;CardGame;KidsGame;LogicGame;RolePlaying;Simulation;SportsGame;StrategyGame;Art;Construction;Music;Languages;Science;ArtificialIntelligence;Astronomy;Biology;Chemistry;ComputerScience;DataVisualization;Economy;Electricity;Geography;Geology;Geoscience;History;ImageProcessing;Literature;Math;NumericalAnalysis;MedicalSoftware;Physics;Robotics;Sports;ParallelComputing;Amusement;Archiving;Compression;Electronics;Emulator;Engineering;FileTools;TerminalEmulator;Filesystem;Monitor;Security;Accessibility;Calculator;Clock;TextEditor;Documentation;Core"
#define FREEDESKTOP_IGNORE_CATEGORIES "KDE;GNOME;Qt;Motif;Java;ConsoleOnly"

struct InstallFile
{
  QString inputFile;
  QString outputFile;
  bool bAbsInputPath;

  InstallFile(const QString &in, const QString &out = QString(), bool absPath = false)
    : inputFile(in), outputFile(out), bAbsInputPath(absPath)
  {}
  bool operator==(const InstallFile &o)
  {
    return (inputFile == o.inputFile && outputFile == o.outputFile);
  }
  friend QDebug operator<<(QDebug, const InstallFile &);
  friend QDebug operator<<(QDebug, QList<InstallFile> &);
};

bool parseQtIncludeFiles(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude);

typedef struct {
    QString shortDesc;
    QString longDesc;
    QString categories;
    QString requires;
} HintFileDescriptor;

bool parseHintFile(const QString &hintFile,HintFileDescriptor &pkg);
bool parseHintFile(const QByteArray &ba, HintFileDescriptor &pkg);

bool findExecutables(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude, bool debugExe=false);

bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList, bool verbose=false);
bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QString &exclude = QString(), bool verbose=false);

bool readDesktopFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeDesktopFile(QIODevice &device, const QSettings::SettingsMap &map);

bool createStartMenuEntries(const QString &dir, const QString &installDir, const QString &category=QString());
bool removeStartMenuEntries(const QString &dir, const QString &category=QString());

typedef enum RegKey { hKEY_CURRENT_USER, hKEY_LOCAL_MACHINE, hKEY_CLASSES_ROOT };
typedef enum RegType { qt_unknown = -1, qt_String, qt_ExpandedString, qt_DWORD, qt_BINARY, qt_MultiString };
QVariant getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok = false);
bool setWin32RegistryValue(const RegKey& akey, const QString& subKey, const QString& item, const QVariant& value, RegType pqvType = qt_unknown);
bool delWin32RegistryValue(const RegKey& akey, const QString& subKey);
// try to delete a file, if not possible, move to root/tmp/deleteme
bool deleteFile(const QString &root, const QString &fn);
// create a md5 hash from file contents
QByteArray md5Hash(QFile &f);

void setMessageHandler();

#endif
