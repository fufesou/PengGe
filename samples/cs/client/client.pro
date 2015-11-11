TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../../src/common ../../../src/cs/common ../../../src/cs/client

HEADERS += \
    ../../../src/common/bufarray.h \
    ../../../src/common/buflist.h \
    ../../../src/common/error.h \
    ../../../src/common/lightthread.h \
    ../../../src/common/list.h \
    ../../../src/common/macros.h \
    ../../../src/common/msgwrap.h \
    ../../../src/common/sock_types.h \
    ../../../src/common/sock_wrap.h \
    ../../../src/common/timespan.h \
    ../../../src/common/utility_wrap.h \
    ../../../src/cs/client/client.h \
    ../../../src/cs/client/client_msgdispatch.h \
    ../../../src/cs/common/msgpool.h \
    ../../../src/cs/common/msgpool_dispatch.h \
    ../../../src/cs/common/unprtt.h


SOURCES += \
    main.c \
    ../../../src/common/bufarray.c \
    ../../../src/common/buflist.c \
    ../../../src/common/error.c \
    ../../../src/common/global.c \
    ../../../src/common/lightthread.c \
    ../../../src/common/list.c \
    ../../../src/common/msgwrap.c \
    ../../../src/common/sock_wrap.c \
    ../../../src/common/timespan.c \
    ../../../src/common/utility_wrap.c \
    ../../../src/cs/client/client.c \
    ../../../src/cs/client/client_msgdispatch.c \
    ../../../src/cs/client/client_udp.c \
    ../../../src/cs/client/unix_client_sendrecv.c \
    ../../../src/cs/client/win_client_sendrecv.c \
    ../../../src/cs/common/msgpool.c \
    ../../../src/cs/common/msgpool_dispatch.c \
    ../../../src/cs/common/rtt.c


win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}
