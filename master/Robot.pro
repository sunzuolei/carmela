#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T16:24:06
#
#-------------------------------------------------

QT       += core gui

TARGET = Robot
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
    robotdriver.cpp
include(qextserialport/qextserialport.pri)

HEADERS  += \
            mainwindow.h \
    robotdriver.h \
    Data.h \
    command.h


FORMS += \
            mainwindow.ui
