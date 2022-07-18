/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>
** Copyright (C) 2006-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
**
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "registry.h"

#ifdef Q_OS_WIN32
#include <QtDebug>
#include <windows.h>
#include <shlwapi.h>

#if defined(__MINGW32__)
# define WIN32_CAST_CHAR (WCHAR*)
#else
# define WIN32_CAST_CHAR (LPCWSTR)
#endif

#define MAX_KEY_LENGTH 255

/**
 \return a value from MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
*/
QVariant getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok)
{
#define FAILURE { if (ok) *ok = false; return QString(); }
    if (ok)
        *ok = true;

    if (subKey.isEmpty())
        FAILURE;

    HKEY key;
    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: FAILURE;
    }

    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;

    if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, KEY_READ, &hKey))
        FAILURE;

    if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, NULL, &dwSize))
        FAILURE;

    QVariant res;
    switch(dwType) {
        case REG_MULTI_SZ:
            // FIXME: this should give a QStringList; to be implemented later
            FAILURE;
            break;
        case REG_EXPAND_SZ:
        case REG_SZ: {
            QByteArray ba(dwSize * 2, 0);
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)ba.data(), &dwSize))
                FAILURE;
            res = QString::fromUtf16( (unsigned short*)ba.data() );
            break;
        }
        case REG_DWORD: {
            qint32 dwVal;
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)&dwVal, &dwSize))
                FAILURE;
            res = dwVal;
            break;
        }
        case REG_BINARY: {
            QByteArray ba(dwSize, 0);
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)ba.data(), &dwSize))
                FAILURE;
            res = ba;
            break;
        }
        default:
            FAILURE;
    }
    RegCloseKey(hKey);

    return res;
}

/**
 \insert a value into MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
 --SE
*/
bool setWin32RegistryValue(const RegKey& akey, const QString& subKey, const QString& item, const QVariant& value, RegType qvType)
{
    HKEY key;
    HKEY hKey;
    DWORD dwResult;
    DWORD dwType;
    DWORD dwSize;
    DWORD lt;
    BYTE *lpData;

    if (subKey.isEmpty()) return false;

    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: return false;
    }
/*
    it needs to be sure that QVariant can be converted to the Registry Data Types REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_SZ
*/

    if(qvType == qt_unknown) {
        // test type of value to find good registry data type
        switch(value.type()) {
            case QVariant::Date:
            case QVariant::DateTime:
            case QVariant::Time:
            case QVariant::String:        qvType = qt_String; break;
            case QVariant::ByteArray:    qvType = qt_BINARY; break;
            case QVariant::ULongLong:
            case QVariant::LongLong:
            case QVariant::Bool:
            case QVariant::UInt:
            case QVariant::Int:            qvType = qt_DWORD; break;
            case QVariant::StringList:    qvType = qt_MultiString; break;
            default:                    return false;
        }
    }

    switch(qvType) {
        case qt_BINARY:             dwType = REG_BINARY;
                                    lpData = (BYTE *) value.toString().toLocal8Bit().data();
                                    dwSize = sizeof(char) * value.toString().toLocal8Bit().length();
                                    break;
        case qt_DWORD:              dwType = REG_DWORD;
                                    lt = value.toInt();             // thinking that it will return qint32 == long signed int(16)!!!!
                                    lpData = (BYTE *)& lt;
                                    dwSize = sizeof(DWORD);
                                    break;
        case qt_String:             dwType = REG_SZ;
                                    lpData = (BYTE *) value.toString().utf16();
                                    dwSize = 2 * sizeof(char) * value.toString().length();
                                    break;
        case qt_ExpandedString:     dwType = REG_EXPAND_SZ;
                                    lpData = (BYTE *) value.toString().utf16();
                                    dwSize = 2 * sizeof(char) * value.toString().length();
                                    break;
        case qt_MultiString:        dwType = REG_MULTI_SZ;
//FIXME: useful for QStringList - to be implemented later
                                    return false;
                                    break;
        default:                    return false;
    }

    if (ERROR_SUCCESS != RegCreateKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwResult))
        return false;

    if (ERROR_SUCCESS != RegSetValueExW(hKey, WIN32_CAST_CHAR item.utf16(), 0, dwType, lpData, dwSize))
        return false;
    RegCloseKey(hKey);
    return true;
}

/**
 \delete a value from MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 --SE
*/

bool delWin32RegistryValue(const RegKey& akey, const QString& subKey)
{
    HKEY key;
    HKEY hKey;
    DWORD dwSubKeys;
    DWORD dwValues;
    DWORD keyLength;
    QString keyName;
    int i;

    if (subKey.isEmpty()) {
        qDebug() << "empty key";
        return false;
    }

    QString hSubKey = subKey.right(subKey.length() - subKey.lastIndexOf("\\") - 1);
    QString lSubKey = subKey.left(subKey.lastIndexOf("\\"));

    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: return false;
    }


    if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
        // is it a value instead?
        if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR lSubKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
            qDebug() << "Could not open registry Key for writing";
            return false;
        } else {
            if (ERROR_SUCCESS != RegDeleteValueW(hKey, WIN32_CAST_CHAR hSubKey.utf16())) {
                qDebug() << "Could not delete registry value" << lSubKey << hSubKey;
                return false;
            }
            RegCloseKey(hKey);
        }
    } else {
        if( ERROR_SUCCESS != RegQueryInfoKeyW(hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, &dwValues, NULL, NULL, NULL, NULL) ){
            qDebug() << "Could not query key" << subKey;
            return false;
        } else {
            for( i = dwSubKeys - 1; i >= 0; i-- ) {
                keyLength = MAX_KEY_LENGTH;
                keyName.resize(MAX_KEY_LENGTH);
                if(ERROR_SUCCESS != RegEnumKeyExW(hKey, (DWORD) i, (wchar_t *) keyName.data(), &keyLength, NULL, NULL, NULL, NULL)) {
                    qDebug() << "Could not enumerate registry key" << subKey << "for keys";
                    return false;
                }
                keyName.resize(keyLength);
                if(ERROR_SUCCESS != SHDeleteKeyW(hKey, WIN32_CAST_CHAR keyName.utf16())) {
                    qDebug() << "Could not delete Key";
                    return false;
                };
            }
            for( i = dwValues - 1; i >= 0; i-- ) {
                keyLength = MAX_KEY_LENGTH;
                keyName.resize(MAX_KEY_LENGTH);
                if(ERROR_SUCCESS != RegEnumValueW(hKey, (DWORD) i, (wchar_t *) keyName.data(), &keyLength, NULL, NULL, NULL, NULL)) {
                    qDebug() << "Could not enumerate registry key" << subKey << "for values";
                    return false;
                }
                keyName.resize(keyLength);
                if (ERROR_SUCCESS != RegDeleteValueW(hKey, WIN32_CAST_CHAR keyName.utf16())) {
                    qDebug() << "Could not delete registry value" << subKey << keyName;
                    return false;
                }
            }
        }
        RegCloseKey(hKey);
        if(lSubKey.isEmpty()) return true;
        if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR lSubKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
            qDebug() << "Could not open registry key" << lSubKey << "for writing";
            return false;
        } else {
            if (ERROR_SUCCESS != RegDeleteKeyW(hKey, WIN32_CAST_CHAR hSubKey.utf16())) {
                qDebug() << "Could not delete registry key" << hSubKey;
                return false;
            }
            RegCloseKey(hKey);
        }
    }

    return true;
}
#endif  // Q_OS_WIN32

