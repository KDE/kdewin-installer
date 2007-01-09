/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
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
#ifndef MIRRORS_H
#define MIRRORS_H

#include <QObject>
#include <QStringList>
class QString;
class QStringList;
class QByteArray;
class QFile;

class Mirrors /* : public QObject */
{

    //Q_OBJECT

public:
    enum MirrorType { KDE = 1 ,CYGWIN= 2 };

    Mirrors(MirrorType type);
    ~Mirrors();
    QStringList get
        ();

protected:
    bool parse(const QString &fileName);
    bool parse(const QByteArray &data);
    bool parse(QIODevice *ioDev);
    QStringList getList()
    {
        return m_mirrorList;
    }

private:
    MirrorType m_type;
    QStringList m_mirrorList;
};

#endif
