/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Mar 13 20:27:17 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainWindow
{
public:
    QTextBrowser *textBrowser;
    QLabel *imageLabel;
    QLabel *imagelabel_2;
    QWidget *widget;

    void setupUi(QWidget *mainWindow)
    {
        if (mainWindow->objectName().isEmpty())
            mainWindow->setObjectName(QString::fromUtf8("mainWindow"));
        mainWindow->resize(624, 407);
        textBrowser = new QTextBrowser(mainWindow);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 10, 601, 192));
        imageLabel = new QLabel(mainWindow);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        imageLabel->setGeometry(QRect(10, 220, 191, 181));
        imagelabel_2 = new QLabel(mainWindow);
        imagelabel_2->setObjectName(QString::fromUtf8("imagelabel_2"));
        imagelabel_2->setGeometry(QRect(200, 220, 201, 181));
        widget = new QWidget(mainWindow);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(399, 219, 211, 181));

        retranslateUi(mainWindow);

        QMetaObject::connectSlotsByName(mainWindow);
    } // setupUi

    void retranslateUi(QWidget *mainWindow)
    {
        mainWindow->setWindowTitle(QApplication::translate("mainWindow", "Form", 0, QApplication::UnicodeUTF8));
        imageLabel->setText(QString());
        imagelabel_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class mainWindow: public Ui_mainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
