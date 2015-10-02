TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../common

HEADERS += \
    ../common/utility.h \
    server_udp.h

SOURCES += \
    ../common/utility.c \
    main.c \
    server_udp.c

LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32

include(deployment.pri)
qtcAddDeployment()

