#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T20:48:12
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Database_GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagedialog.cpp \
    showpoints.cpp

HEADERS  += mainwindow.h \
    imagedialog.h \
    showpoints.h

FORMS    += mainwindow.ui \
    imagedialog.ui \
    showpoints.ui
