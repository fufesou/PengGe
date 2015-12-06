TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):
DEFINES +=  _DEBUG

CONFIG += -mrtd

INCLUDEPATH += 	../../../src/include

HEADERS += \
    ../../../src/include/common/bufarray.h \
    ../../../src/include/common/buflist.h \
    ../../../src/include/common/clearlist.h \
    ../../../src/include/common/config_macros.h \
    ../../../src/include/common/cstypes.h \
    ../../../src/include/common/error.h \
    ../../../src/include/common/lightthread.h \
    ../../../src/include/common/list.h \
    ../../../src/include/common/macros.h \
    ../../../src/include/common/msgwrap.h \
    ../../../src/include/common/sock_types.h \
    ../../../src/include/common/sock_wrap.h \
    ../../../src/include/common/timespan.h \
    ../../../src/include/common/utility_wrap.h \
    ../../../src/include/am/account.h \
    ../../../src/include/am/account_file.h \
    ../../../src/include/am/account_login.h \
    ../../../src/include/am/account_macros.h \
    ../../../src/include/am/client_account.h \
    ../../../src/include/am/server_account.h \
    ../../../src/include/cs/client.h \
    ../../../src/include/cs/msgpool.h \
    ../../../src/include/cs/msgpool_dispatch.h \
    ../../../src/include/cs/server.h \
    ../../../src/include/cs/unprtt.h


SOURCES += \
    main.c \
    ../../../src/common/bufarray.c \
    ../../../src/common/buflist.c \
    ../../../src/common/error.c \
    ../../../src/common/global.c \
    ../../../src/common/lightthread.c \
    ../../../src/common/list.c \
    ../../../src/common/msgwrap.c \
    ../../../src/common/sock_wrap.c \
    ../../../src/common/timespan.c \
    ../../../src/common/utility_wrap.c \
    ../../../src/cs/client/client.c \
    ../../../src/cs/client/unix_client_sendrecv.c \
    ../../../src/cs/client/win_client_sendrecv.c \
    ../../../src/cs/common/msgpool.c \
    ../../../src/cs/common/msgpool_dispatch.c \
    ../../../src/cs/common/rtt.c \
    ../../../src/am/common/account.c \
    ../../../src/am/common/account_file.c \
    ../../../src/am/client/client_account.c \
    ../../../src/am/server/account_login.c \
    ../../../src/am/server/server_account.c \
    ../../../src/common/clearlist.c


win32 {
    LIBS += -LD:/Qt/Qt5.4.2/Tools/mingw491_32/i686-w64-mingw32/lib -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

RESOURCES += \
    data.qrc
