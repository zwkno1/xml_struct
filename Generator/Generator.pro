#-------------------------------------------------
#
# Project created by QtCreator 2016-02-26T20:57:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Generator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    inputdialog.cpp \
    parser.cpp

HEADERS  += mainwindow.h \
    inputdialog.h \
    parser.h

FORMS    += mainwindow.ui \
    inputdialog.ui

INCLUDEPATH += ../
QMAKE_CXXFLAGS += -std=c++14
