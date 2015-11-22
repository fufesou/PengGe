TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):
DEFINES +=  _DEBUG


INCLUDEPATH += 	../../src/common


SOURCES += \
    test_thread.c \
    ../../src/common/lightthread.c \
    ../../src/common/timespan.c


win32 {
    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
