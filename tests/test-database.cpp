/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
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

#include "database.h"
#include "settings.h"

// this should go into Package class sometime in the future
static Package::Type StringToPackageType(const QString &_type)
{
    QString type = _type.toLower();
    
    if (type == "bin")
        return Package::BIN;
    else if (type == "lib")
        return Package::LIB;
    else if (type == "doc")
        return Package::DOC;
    else if (type == "src")
        return Package::SRC;
    else 
        return Package::NONE;
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Settings &s = Settings::getInstance();
    
    Database db; 
    db.setRoot(s.installDir());

    db.readFromDirectory();
    db.listPackages();
    QStringList args = app.arguments();
    QString pkgName = args.count() >= 2 ? args.at(1) : "test";
    QString pkgType = args.count() == 3 ? args.at(2) : "bin";
    
    QStringList files = db.getPackageFiles(pkgName,StringToPackageType(pkgType));
    foreach(QString file, files)
        qDebug() << file;
    
    return 0;
}

