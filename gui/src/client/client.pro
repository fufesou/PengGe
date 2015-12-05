#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T20:43:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuiClient
TEMPLATE = app

INCLUDEPATH += ../common


SOURCES += \
    main.cpp \
    loginwindow.cpp \
    ../common/wordsbutton.cpp \
    mainwidget.cpp \
    clientcontroller.cpp \
    logingwidget.cpp \
    ../common/chatwidget.cpp

HEADERS  += \
    loginwindow.h \
    ../common/wordsbutton.h \
    mainwidget.h \
    clientcontroller.h \
    logingwidget.h \
    ../common/chatwidget.h \
    ../common/guimacros.h \
    ../common/guitypes.h

RESOURCES += \
    ../resources/qss.qrc \
    ../resources/img.qrc
