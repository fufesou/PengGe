TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

HEADERS += \
    server_udp.h \
    ../common/udp_utility.h

SOURCES += \
    main.c \
    server_udp.c \
    ../common/udp_utility.c

LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32

include(deployment.pri)
qtcAddDeployment()

