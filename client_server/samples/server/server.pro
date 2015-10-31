TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

INCLUDEPATH += ../../src/common ../../src/client

HEADERS += \

SOURCES += \
#    main.c \


win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

include(deployment.pri)
qtcAddDeployment()

