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
    ../../../src/include/common/processlist.h \
    ../../../src/include/common/msgprocess.h \
    ../../../src/include/common/config_macros.h \
    ../../../src/include/common/error.h \
    ../../../src/include/common/jxiot.h \
    ../../../src/include/common/sock_types.h \
    ../../../src/include/common/sock_wrap.h \
    ../../../src/include/common/utility_wrap.h \
    ../../../src/include/cs/client.h \
    ../../../src/include/am/client_account.h


SOURCES += \
    ../../../samples/cs/client/main.c

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_win
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_win
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/build-pgcs-Desktop_Qt_5_5_1_MinGW_32bit-Debug/debug -lpgcsd_win
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcs_unix
# else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/ -lpgcsd_unix
else:unix:CONFIG(debug, debug|release): LIBS += \
    -L$$PWD/../../../build/build-pgcs-Desktop_Qt_5_5_1_GCC_64bit-Debug/ \
    -L$$PWD/../../../build/build-pgcs-Desktop_Qt_5_5_1_GCC_64bit-Debug/ \
    -lpgcsd_unix

win32 {
    LIBS += -lwsock32 -lWinmm -lWs2_32
}

unix {
    LIBS += -pthread
}

RESOURCES += \
    ../../../samples/cs/client/data.qrc

