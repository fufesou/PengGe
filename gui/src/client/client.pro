#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T20:43:51
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuiClient
TEMPLATE = app

CONFIG += c++11

win32:DEFINES += WIN32

INCLUDEPATH += ../common
INCLUDEPATH += 	$$PWD/../../../src/include
INCLUDEPATH += $$PWD/../../../build/include


SOURCES += \
    main.cpp \
    mainwidget.cpp \
    clientcontroller.cpp \
    requestwidget.cpp \
    requestingwidget.cpp \
    loginwidget.cpp \
    loginingwidget.cpp \
    registerwidget.cpp \
    registeringwidget.cpp \
    verifywidget.cpp \
    verifyingwidget.cpp \
    ../common/wordsbutton.cpp \
    ../common/chatwidget.cpp \
    ../common/displayiteminfo.cpp \
    ../common/displayitemwidget.cpp \
    ../common/listwidget.cpp \
    ../common/selfinfowidget.cpp

HEADERS  += \
    mainwidget.h \
    clientcontroller.h \
    requestwidget.h \
    requestingwidget.h \
    loginwidget.h \
    loginingwidget.h \
    registerwidget.h \
    registeringwidget.h \
    verifywidget.h \
    verifyingwidget.h \
    ../common/guithread.h \
    ../common/wordsbutton.h \
    ../common/chatwidget.h \
    ../common/guimacros.h \
    ../common/guitypes.h \
    ../common/displayiteminfo.h \
    ../common/displayitemwidget.h \
    ../common/listwidget.h \
    ../common/selfinfowidget.h

RESOURCES += \
    ../resources/qss.qrc \
    ../resources/img.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_win
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_win
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../samples/build-pgcs-Desktop_Qt_5_4_2_MinGW_32bit-Debug/debug -lpgcsd_win
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_unix
# else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_unix
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../samples/build-pgcs-Desktop_Qt_5_4_2_GCC_64bit-Debug/ -lpgcsd_unix

win32 {
    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
