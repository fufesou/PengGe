TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

INCLUDEPATH += ../common

HEADERS += \
    server_udp.h \
    server_sendrecv.h \
    ../common/sock_types.h \
    ../common/bufarray.h \
    ../common/macros.h \
    msgdispatch.h \
    server_servroutine.h \
    ../common/sendrecv_pool.h \
    ../common/msgwrap.h \
    ../common/error.h \
    ../common/lightthread.h \
    ../common/sock_wrap.h \
    ../common/timespan.h \
    ../common/utility_wrap.h

SOURCES += \
#    main.c \
    test/wsaserver.c \
    server_udp.c \
    server_sendrecv.c \
    ../common/bufarray.c \
    server_servroutine.c \
    msgdispatch.c \
    ../common/sendrecv_pool.c \
    ../common/msgwrap.c \
    ../common/error.c \
    ../common/global.c \
    ../common/lightthread.c \
    ../common/sock_wrap.c \
    ../common/timespan.c \
    ../common/utility_wrap.c


win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

include(deployment.pri)
qtcAddDeployment()

