TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

SOURCES += \
    main_arraybuf.c \
    # main_listbuf.c \
    buflist.c \
    list.c \
    bufarray.c

HEADERS += \
    buflist.h \
    list.h \
    macros.h \
    bufarray.h

