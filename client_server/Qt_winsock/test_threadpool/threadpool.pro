TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

SOURCES += \
    # win_arraybuf.c \
    # win_listbuf.c \
    main_arraybuf.c \
    ../common/bufarray.c \
    ../common/buflist.c \
    ../common/lightthread.c \
    ../common/list.c \
    ../common/sendrecv_pool.c \
    ../common/timespan.c \
    ../common/utility_wrap.c

HEADERS += \
    ../common/bufarray.h \
    ../common/buflist.h \
    ../common/lightthread.h \
    ../common/list.h \
    ../common/macros.h \
    ../common/sendrecv_pool.h \
    ../common/sock_types.h \
    ../common/timespan.h \
    ../common/utility_wrap.h

win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
