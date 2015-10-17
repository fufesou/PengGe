TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):DEFINES +=  _DEBUG

SOURCES += \
    main.c \
    buflist.c \
    list.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    buflist.h \
    list.h \
    macros.h

