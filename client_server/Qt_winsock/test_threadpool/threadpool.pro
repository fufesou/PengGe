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
    ../common/timespan.c

HEADERS += \
    ../common/bufarray.h \
    ../common/buflist.h \
    ../common/lightthread.h \
    ../common/list.h \
    ../common/macros.h \
    ../common/sendrecv_pool.h \
    ../common/sock_types.h \
    ../common/timespan.h

