/****************************************************************************
**
** Copyright (C) 2007 Ralf Habacker. All rights reserved.
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

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "database.h"
#include "package.h"

Database::Database()
        : QObject()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

}

Database::~Database()
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif
    qDeleteAll(m_database);
}


void Database::addPackage(const Package &package)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__ << package.toString();
#endif

    m_database.append(new Package(package));
}

Package *Database::getPackage(const QString &pkgName, const QByteArray &version)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QList<Package*>::iterator it = m_database.begin();
    for ( ; it != m_database.end(); ++it) {
        if ((*it)->name() == pkgName) {
            if(!version.isEmpty() && (*it)->version() != version)
                continue;
            return (*it);
        }
    }
    return NULL;
}

void Database::listPackages(const QString &title)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    qDebug() << title;
    QList<Package*>::iterator it;
    for (it = m_database.begin(); it != m_database.end(); ++it)
        qDebug() << (*it)->toString(true," - ");
}

bool Database::readFromDirectory(const QString &dir)
{
#ifdef DEBUG
    qDebug() << __FUNCTION__;
#endif

    QDir d(dir);
    d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    d.setNameFilters(QStringList("*.ver"));
    d.setSorting(QDir::Name);

    QFileInfoList list = d.entryInfoList();
    QFileInfo fi;
     
    // wget-1.10.1-bin.ver
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fi = list[i];        
        QString baseName = fi.fileName().remove(".ver");
        QStringList parts = baseName.split('-');
        if(parts.size() < 3) {
            qDebug() << "can't parse filename " << baseName;
            continue;
        }
        QString pkgName = parts[0];
		QString pkgVersion;
		QString pkgType;
		if (parts.size() == 5)
		{
				pkgName = parts[0] + "-" + parts[1];
				pkgVersion = parts[2] + '-' + parts[3];
				pkgType = parts[4];
		}
		else if (parts.size() == 4)
		{
			if (parts[1][0].isLetter())
			{
				pkgName += "-" + parts[1];
				pkgVersion = parts[2];
			}
			else
				pkgVersion = parts[1] + '-' + parts[2];
			pkgType = parts[3];
		}
		else 
		{
			pkgVersion = parts[1];
			// aspell-0.50.3-3
			if (parts[2][0].isNumber())
			{
				pkgVersion += "-" + parts[2];
				pkgType = "bin";
			}
			else
				pkgType = parts[2];
		}
		Package *pkg;
        if (pkg = getPackage(pkgName)) 
        {
            Package::PackageItem pi;
            pi.bInstalled =true; 
			pi.setContentType(pkgType);
            pkg->add(pi);
        }
        else 
        {
            Package *pkg =  new Package;
            pkg->setName(pkgName);
			pkg->setVersion(pkgVersion);
            Package::PackageItem pi;
            pi.bInstalled = true;
			pi.setContentType(pkgType);
			pkg->add(pi);
            m_database.append(pkg);
        }
    }        
    emit configLoaded();
    return true;
}

// returns version file name of package item e.g. xyz-1.2.3-bin.ver
QString Database::versionFileName(const QString &name, const QString &version, Package::Type type)
{
    Package *pkg;
	if (pkg = getPackage(name)) 
		return pkg->name() + "-" + pkg->version() + "-" + Package::typeToString(type) +".ver"; 
	else
		return QString();
}

// returns manifest file name of package item e.g. xyz-1.2.3-bin.mft
QString Database::manifestFileName(const QString &name, const QString &version, Package::Type type)
{
    Package *pkg;
	if (pkg = getPackage(name)) 
		return name + "-" + version + "-" + Package::typeToString(type) +".mft";
	else
		return QString();

}



void Database::dump(const QString &title)
{
    qDebug() << "class Database dump: " << title;
    QList<Package*>::ConstIterator it = m_database.constBegin();
    for ( ; it != m_database.constEnd(); ++it)
        (*it)->dump();
}

#include "database.moc"
