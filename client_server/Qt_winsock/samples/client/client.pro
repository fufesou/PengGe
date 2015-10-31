TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

HEADERS += \
    unprtt.h \
    udp_send_recv.h \
    ../common/udp_utility.h \
    ../common/error.h \
    ../common/macros.h \
    ../common/sock_types.h \
    ../common/sock_wrap.h \
    ../common/msgwrap.h \
    ../common/sendrecv_pool.h \
    ../common/bufarray.h \
    ../common/lightthread.h \
    ../common/timespan.h

SOURCES += \
    client_udp.c \
    main.c \
#    test\wsaclinet.c \
    rtt.c \
    ../common/udp_utility.c \
    ../common/error.c \
    ../common/global.c \
    ../common/sock_wrap.c \
    ../common/msgwrap.c \
    ../common/sendrecv_pool.c \
    ../common/bufarray.c \
    ../common/lightthread.c \
    ../common/timespan.c

win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

include(deployment.pri)
qtcAddDeployment()

