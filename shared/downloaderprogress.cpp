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

#include <QDebug>

#include "downloaderprogress.h"

#ifdef USE_GUI

#include "complexwizard.h"

DownloaderProgress::DownloaderProgress(ComplexWizard *parent) 
{ 
    statusLabel = new QLabel();
    progress = new QProgressBar(parent); 

    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(progress);

    titleLabel = new QLabel();
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(statusLayout);
    setLayout(mainLayout);
    hide();
}

DownloaderProgress::~DownloaderProgress()
{
	delete titleLabel;
	delete statusLabel;
	delete progress;
}

void DownloaderProgress::hide() 
{ 
	titleLabel->hide(); 
	statusLabel->hide(); 
	progress->hide(); 
}

void DownloaderProgress::setTitle(const QString &label) 
{ 
	titleLabel->setText(label); 
}

void DownloaderProgress::setStatus(const QString &label) 
{ 
	statusLabel->setText(label); 
}

void DownloaderProgress::show() 
{ 
	titleLabel->show(); 
	statusLabel->show();
	progress->show(); 
}

void DownloaderProgress::setMaximum(int value) 
{ 
	progress->setMaximum(value); 
} 

void DownloaderProgress::setValue(int value)   
{ 
	progress->setValue(value); 
} 

#else // console implementation 

DownloaderProgress::DownloaderProgress(QObject *parent)
{
    hide();
}

void DownloaderProgress::hide() 
{ 
	visible = false; 
}

void DownloaderProgress::setTitle(const QString &title) 
{ 
	qDebug() << title; 
}

void DownloaderProgress::setStatus(const QString &status) 
{ 
	qDebug() << status; 
}

void DownloaderProgress::setMaximum(int value) 
{
} 

void DownloaderProgress::setValue(int value) 
{ 
	int unit = value/10240;
	if (oldunit != value/10240) {
		if (visible)
			putchar('.');
		oldunit = unit;
	}
}

void DownloaderProgress::show() 
{ 
	visible = true; 
}

#endif



