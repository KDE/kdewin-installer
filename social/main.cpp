/*
 * main.cpp
 *
 *  Created on: May 20, 2011
 *      Author: tudalex
 */

#include <QObject>
#include <QApplication>
#include <QTextEdit>
#include <debug.h>

#include "mainwindow.h"
int main(int argv, char **args)
{
    setMessageHandler();
    QApplication app(argv,args);
    MainWindow* window = new MainWindow();
    window->show();
    return app.exec();
}

