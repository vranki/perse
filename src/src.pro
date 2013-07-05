#-------------------------------------------------
#
# Project created by QtCreator 2013-07-05T20:29:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = perse
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    udev.cpp \
    rulefile.cpp

HEADERS  += mainwindow.h \
    udev.h \
    rulefile.h

FORMS    += mainwindow.ui

LIBS += -ludev
