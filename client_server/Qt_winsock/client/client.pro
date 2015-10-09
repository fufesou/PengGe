TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

HEADERS += \
    client_udp.h \
    unprtt.h \
    udp_send_recv.h \
    ../common/udp_utility.h

SOURCES += \
    client_udp.c \
    main.c \
    rtt.c \
    udp_send_recv.c \
    ../common/udp_utility.c

LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32

include(deployment.pri)
qtcAddDeployment()

