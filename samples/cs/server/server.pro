TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, release|debug):
DEFINES +=  _DEBUG

INCLUDEPATH += 	../../../src/include


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


win32 {
    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/release/ -lpgcs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/debug/ -lpgcs
else:unix: LIBS += -L$$PWD/../../../build/lib/ -lpgcs

INCLUDEPATH += $$PWD/../../../build/include
DEPENDPATH += $$PWD/../../../build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../build/lib/release/libpgcs.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../build/lib/debug/libpgcs.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../build/lib/release/pgcs.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../build/lib/debug/pgcs.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../build/lib/libpgcs.a
