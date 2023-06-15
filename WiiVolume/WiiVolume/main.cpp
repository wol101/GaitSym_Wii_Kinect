/*
 *  MainWindow.cpp
 *  WiiVolume
 *
 *  Created by Bill Sellers on 12/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
