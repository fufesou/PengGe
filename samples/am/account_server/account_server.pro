TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += ../../../src/common ../../../src/am/common ../../../src/am/server

SOURCES += main.c \
    ../../../src/common/error.c \
    ../../../src/common/global.c \
    ../../../src/common/lightthread.c \
    ../../../src/common/list.c \
    ../../../src/common/sock_wrap.c \
    ../../../src/common/timespan.c \
    ../../../src/common/utility_wrap.c \
    ../../../src/am/server/server_account.c \
    ../../../src/am/common/account.c \
    ../../../src/am/common/account_file.c \
    ../../../src/am/client/client_account.c \
    ../../../src/am/server/account_login.c

win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

HEADERS += \
    ../../../src/common/error.h \
    ../../../src/common/lightthread.h \
    ../../../src/common/list.h \
    ../../../src/common/macros.h \
    ../../../src/common/msgwrap.h \
    ../../../src/common/sock_types.h \
    ../../../src/common/sock_wrap.h \
    ../../../src/common/timespan.h \
    ../../../src/common/utility_wrap.h \
    ../../../src/am/server/server_account.h \
    ../../../src/am/common/account.h \
    ../../../src/am/common/account_file.h \
    ../../../src/am/common/account_macros.h \
    ../../../src/am/client/client_account.h \
    ../../../src/am/server/account_login.h
