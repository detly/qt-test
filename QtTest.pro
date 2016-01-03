#-------------------------------------------------
#
# Project created by QtCreator 2015-12-22T11:08:44
#
#-------------------------------------------------

QT       += core gui

CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtTest
TEMPLATE = app


SOURCES += main.cpp \
    hexviewer.cpp \
    filereader.cpp

HEADERS  += \
    filereader.h \
    hexviewer.h

FORMS    += \
    hexviewer.ui
