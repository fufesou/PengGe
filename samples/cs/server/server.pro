TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):
DEFINES +=  _DEBUG

INCLUDEPATH += 	../../../src/include
INCLUDEPATH += $$PWD/../../../build/include


HEADERS += \
    ../../../src/include/common/clearlist.h \
    ../../../src/include/common/config_macros.h \
    ../../../src/include/common/error.h \
    ../../../src/include/common/macros.h \
    ../../../src/include/common/sock_types.h \
    ../../../src/include/common/sock_wrap.h \
    ../../../src/include/common/utility_wrap.h \
    ../../../src/include/cs/server.h \
    ../../../src/include/am/server_account.h


SOURCES += \
    main.c

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_win
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_win
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_unix
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_unix

win32 {
    LIBS += -lwsock32 -lwinmm -lws2_32
}

unix {
    LIBS += -pthread
}
