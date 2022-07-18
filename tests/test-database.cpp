/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QtDebug>
#include <QCoreApplication>

#include "database.h"
#include "settings.h"



#ifdef TEST_MSI_INSTALLER

#ifndef UNICODE
#define UNICODE
#endif //UNICODE

#ifndef _WIN32_MSI
#define _WIN32_MSI 200
#endif //_WIN32_MSI

#include <stdio.h>
#include <windows.h>
#include <msi.h>

const int cchGUID = 38;

UINT DetermineContextForAllProducts()
{
	WCHAR wszProductCode[cchGUID+1] = {0};
	WCHAR wszAssignmentType[10] = {0};
	DWORD cchAssignmentType = 
			sizeof(wszAssignmentType)/sizeof(wszAssignmentType[0]);
	DWORD dwIndex = 0;

	DWORD cchProductName = MAX_PATH;
	WCHAR* lpProductName = new WCHAR[cchProductName];
	if (!lpProductName)
	{
		return ERROR_OUTOFMEMORY;
	}

	UINT uiStatus = ERROR_SUCCESS;

	// enumerate all visible products
	do
	{
		uiStatus = MsiEnumProducts(dwIndex,
					      wszProductCode);
		if (ERROR_SUCCESS == uiStatus)
		{
			cchAssignmentType = 
				sizeof(wszAssignmentType)/sizeof(wszAssignmentType[0]);
			BOOL fPerMachine = FALSE;
			BOOL fManaged = FALSE;

			// Determine assignment type of product
			// This indicates whether the product
			// instance is per-user or per-machine
			if (ERROR_SUCCESS == 
				MsiGetProductInfo(wszProductCode,INSTALLPROPERTY_ASSIGNMENTTYPE,wszAssignmentType,&cchAssignmentType))
			{
				if (L'1' == wszAssignmentType[0])
					fPerMachine = TRUE;
			}
			else
			{
				// This halts the enumeration and fails. Alternatively the error
				// could be logged and enumeration continued for the
				// remainder of the products
				uiStatus = ERROR_FUNCTION_FAILED;
				break;
			}

			// determine the "managed" status of the product.
			// If fManaged is TRUE, product is installed managed
			// and runs with elevated privileges.
			// If fManaged is FALSE, product installation operations
			// run as the user.
			if (ERROR_SUCCESS != MsiIsProductElevated(wszProductCode,
								         &fManaged))
			{
				// This halts the enumeration and fails. Alternatively the error
				// could be logged and enumeration continued for the
				// remainder of the products
				uiStatus = ERROR_FUNCTION_FAILED;
				break;
			}

			// obtain the user friendly name of the product
			UINT uiReturn = MsiGetProductInfo(wszProductCode,INSTALLPROPERTY_PRODUCTNAME,lpProductName,&cchProductName);
			if (ERROR_MORE_DATA == uiReturn)
			{
				// try again, but with a larger product name buffer
				delete [] lpProductName;

				// returned character count does not include
				// terminating NULL
				++cchProductName;

				lpProductName = new WCHAR[cchProductName];
				if (!lpProductName)
				{
					uiStatus = ERROR_OUTOFMEMORY;
					break;
				}

				uiReturn = MsiGetProductInfo(wszProductCode,INSTALLPROPERTY_PRODUCTNAME,lpProductName,&cchProductName);
			}

			if (ERROR_SUCCESS != uiReturn)
			{
				// This halts the enumeration and fails. Alternatively the error
				// could be logged and enumeration continued for the
				// remainder of the products
				uiStatus = ERROR_FUNCTION_FAILED;
				break;
			}

			// output information
			wprintf(L" Product %s:\n", lpProductName);
			wprintf(L"\t%s\n", wszProductCode);
            			wprintf(L"\tInstalled %s %s\n", 
				fPerMachine ? L"per-machine" : L"per-user",
				fManaged ? L"managed" : L"non-managed");
		}
		dwIndex++;
	}
	while (ERROR_SUCCESS == uiStatus);

	if (lpProductName)
	{
		delete [] lpProductName;
		lpProductName = NULL;
	}

	return (ERROR_NO_MORE_ITEMS == uiStatus) ? ERROR_SUCCESS : uiStatus;
}
#endif


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Settings &s = Settings::instance();
    
    Database db; 
    db.setRoot(s.installDir());

    db.readFromDirectory();
    db.listPackages();
    QStringList args = app.arguments();
    QString pkgName = args.count() >= 2 ? args.at(1) : "test";
    QString pkgType = args.count() == 3 ? args.at(2) : "bin";
    
    QStringList files = db.getPackageFiles(pkgName,FileTypes::fromString(pkgType));

    foreach(QString file, files)
        qDebug() << file;
    
#ifdef TEST_MSI_INSTALLER
	DetermineContextForAllProducts();
#endif
	
    return 0;
}

