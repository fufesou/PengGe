CONFIG(debug, release|debug):

win32:CONFIG(release, debug|release): TARGET = pgcs_win
else:win32:CONFIG(debug, debug|release): TARGET= pgcsd_win
unix:CONFIG(release, debug|release): TARGET = pgcs_unix
else:unix:CONFIG(debug, debug|release): TARGET= pgcsd_unix

TEMPLATE = lib
CONFIG += staticlib


DEFINES +=  _DEBUG

INCLUDEPATH += 	../../src/include

win32 {
#    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
#    LIBS += -pthread
}

HEADERS += \
    ../../src/include/common/jxiot.h \
    ../../src/include/common/bufarray.h \
    ../../src/include/common/buflist.h \
    ../../src/include/common/clearlist.h \
    ../../src/include/common/processlist.h \
    ../../src/include/common/config_macros.h \
    ../../src/include/common/cstypes.h \
    ../../src/include/common/error.h \
    ../../src/include/common/lightthread.h \
    ../../src/include/common/list.h \
    ../../src/include/common/msgwrap.h \
    ../../src/include/common/sock_types.h \
    ../../src/include/common/sock_wrap.h \
    ../../src/include/common/timespan.h \
    ../../src/include/common/utility_wrap.h \
    ../../src/include/cs/client.h \
    ../../src/include/cs/msgpool.h \
    ../../src/include/cs/msgpool_dispatch.h \
    ../../src/include/cs/server.h \
    ../../src/include/cs/unprtt.h \
    ../../src/include/am/account.h \
    ../../src/include/am/account_file.h \
    ../../src/include/am/account_login.h \
    ../../src/include/am/account_macros.h \
    ../../src/include/am/client_account.h \
    ../../src/include/am/server_account.h

SOURCES += \
    ../../src/common/bufarray.c \
    ../../src/common/buflist.c \
    ../../src/common/clearlist.c \
    ../../src/common/processlist.c \
    ../../src/common/error.c \
    ../../src/common/global.c \
    ../../src/common/lightthread.c \
    ../../src/common/list.c \
    ../../src/common/msgwrap.c \
    ../../src/common/sock_wrap.c \
    ../../src/common/timespan.c \
    ../../src/common/utility_wrap.c \
    ../../src/cs/common/msgpool.c \
    ../../src/cs/common/msgpool_dispatch.c \
    ../../src/cs/common/rtt.c \
    ../../src/cs/client/client.c \
    ../../src/cs/client/client_sendrecv.c \
    ../../src/cs/server/server.c \
    ../../src/am/common/account.c \
    ../../src/am/common/account_file.c \
    ../../src/am/client/client_account.c \
    ../../src/am/server/account_login.c \
    ../../src/am/server/server_account.c

