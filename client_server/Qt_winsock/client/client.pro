TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

HEADERS += \
    ../common/utility.h \
    client_udp.h \
    unprtt.h \
    udp_send_recv.h

SOURCES += \
    ../common/utility.c \
    client_udp.c \
    main.c \
    rtt.c \
    udp_send_recv.c

LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32

include(deployment.pri)
qtcAddDeployment()

