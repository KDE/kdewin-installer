/*
 * main.cpp
 *
 *  Created on: May 20, 2011
 *      Author: tudalex
 */

#include <QObject>
#include <QApplication>
#include <QTextEdit>


#include "mainwindow.h"
int main(int argv, char **args)
{
    QApplication app(argv,args,1);
    MainWindow* window = new MainWindow();
    window->show();
    return app.exec();
}

