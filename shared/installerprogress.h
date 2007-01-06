/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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

#ifndef INSTALLERPROGRESS_H
#define INSTALLERPROGRESS_H

#include <QString>

#ifdef USE_GUI

#include <QWidget>

class QLabel;
class QProgressBar;

class InstallerProgress : public QWidget {
	public:
		InstallerProgress(QWidget *parent);
		~InstallerProgress();
		void hide();
		void setTitle(const QString &title);
		void setStatus(const QString &status);
		void setMaximum(int value);
		void setValue(int value);
		void show();
	private:
		QLabel *titleLabel;
		QLabel *statusLabel;
		QProgressBar *progress;
};

#else
class InstallerProgress {
	public:
		InstallerProgress(QObject *parent);
		void hide();
		void setTitle(const QString &title);
		void setStatus(const QString &status);
		void setMaximum(int value);
		void setValue(int value);
		void show();
		
	private:
		int oldunit;
		bool visible;		
};
#endif


#endif 
