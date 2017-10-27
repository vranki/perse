QT += core gui widgets

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

pkexec.files = ../debian/perse-pkexec
pkexec.path = /usr/bin

INSTALLS += target rulefile desktop policy pkexec
