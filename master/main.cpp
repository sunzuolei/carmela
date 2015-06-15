#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QApplication>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindow w;
    w.show();
    return a.exec();
}
