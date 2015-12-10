#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T20:43:51
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuiClient
TEMPLATE = app

INCLUDEPATH += ../common
INCLUDEPATH += 	$$PWD/../../../src/include
INCLUDEPATH += $$PWD/../../../build/include


SOURCES += \
    main.cpp \
    loginwindow.cpp \
    ../common/wordsbutton.cpp \
    mainwidget.cpp \
    clientcontroller.cpp \
    logingwidget.cpp \
    ../common/chatwidget.cpp \
    ../common/displayiteminfo.cpp \
    ../common/displayitemwidget.cpp \
    ../common/listwidget.cpp \
    ../common/selfinfowidget.cpp

HEADERS  += \
    loginwindow.h \
    ../common/wordsbutton.h \
    mainwidget.h \
    clientcontroller.h \
    logingwidget.h \
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
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_win
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_unix
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_unix

win32 {
    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
