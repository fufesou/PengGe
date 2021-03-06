TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += ../../src/common

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

SOURCES += \
    # win_arraybuf.c \
    # win_listbuf.c \
    main_arraybuf.c \
    ../../src/common/bufarray.c \
    ../../src/common/buflist.c \
    ../../src/common/error.c \
    ../../src/common/global.c \
    ../../src/common/lightthread.c \
    ../../src/common/list.c \
    ../../src/common/msgwrap.c \
    ../../src/common/rtt.c \
    ../../src/common/msgpool.c \
    ../../src/common/sock_wrap.c \
    ../../src/common/timespan.c \
    ../../src/common/utility_wrap.c

HEADERS += \
    ../../src/common/bufarray.h \
    ../../src/common/buflist.h \
    ../../src/common/error.h \
    ../../src/common/lightthread.h \
    ../../src/common/list.h \
    ../../src/common/macros.h \
    ../../src/common/msgwrap.h \
    ../../src/common/msgpool.h \
    ../../src/common/sock_types.h \
    ../../src/common/sock_wrap.h \
    ../../src/common/timespan.h \
    ../../src/common/unprtt.h \
    ../../src/common/utility_wrap.h

win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
