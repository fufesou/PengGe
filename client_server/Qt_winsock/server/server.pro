TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

INCLUDEPATH += ../common

HEADERS += \
    server_udp.h \
    ../common/udp_utility.h \
    server_sendrecv.h \
    ../common/udp_types.h \
    ../common/bufarray.h \
    ../common/macros.h \
    msgdispatch.h \
    server_servroutine.h \
    ../common/sendrecv_pool.h \
    ../common/msgunit.h

SOURCES += \
#    main.c \
    test/wsaserver.c \
    server_udp.c \
    ../common/udp_utility.c \
    server_sendrecv.c \
    ../common/bufarray.c \
    server_servroutine.c \
    msgdispatch.c \
    ../common/sendrecv_pool.c \
    ../common/msgunit.c

LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32

include(deployment.pri)
qtcAddDeployment()

