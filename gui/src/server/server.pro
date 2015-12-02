#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T20:43:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += \
        main.cpp \
        loginwindow.cpp \
        ../common/wordsbutton.cpp

HEADERS  += \
        loginwindow.h \
        ../common/wordsbutton.h

RESOURCES += \
    ../resources/qss.qrc
