#-------------------------------------------------
#
# Project created by QtCreator 2013-07-05T20:29:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = perse
target.path = /usr/bin

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

rulefile.files = ../50-perse.rules
rulefile.path = /lib/udev/rules.d

desktop.files = ../perse.desktop
desktop.path = /usr/share/applications

policy.files = ../com.ubuntu.pkexec.perse.policy
policy.path = /usr/share/polkit-1/actions

INSTALLS += target rulefile desktop policy
