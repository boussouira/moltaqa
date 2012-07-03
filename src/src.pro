TEMPLATE = app
APPNAME = moltaqa-lib

TARGET = $$APPNAME

CODECFORSRC = UTF-8
CODECFORTR  = UTF-8

include(src.pri)
include(quazip/quazip.pri)
include(qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp
DESTDIR = ../bin

exists(../../clucene) {
    CLUCENE_SOURCE_PATH = ../../clucene
    CLUCENE_BUILD_PATH = ../../clucene

    message(Using CLucene found at $$CLUCENE_BUILD_PATH)
} else {
    CLUCENE_SOURCE_PATH = $$(CLUCENE_SOURCE_PATH)
    CLUCENE_BUILD_PATH = $$(CLUCENE_BUILD_PATH)
}

win32 {
    win32-msvc* {
        CONFIG(debug, debug|release) {
            CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin/debug
            CLUCENE_LIB_SUFFIX = "d"
        }

        CONFIG(release, debug|release) {
            CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin/release
        }

        DEFINES +=  _CRT_SECURE_NO_DEPRECATE \
                    _CRT_NONSTDC_NO_DEPRECATE
    }

    LIBS += -luser32
}

unix {
    CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin
}

exists(../.git/HEAD) {
    GITVERSION = $$system(git log -n1 --pretty=format:%h)
    !isEmpty(GITVERSION) {
        GITCHANGENUMBER = $$system(git log --pretty=format:%h | wc -l)
        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
        DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
    }
}

DEFINES += _REENTRANT _UCS2 _UNICODE

INCLUDEPATH += $$CLUCENE_SOURCE_PATH/src/core \
        $$CLUCENE_SOURCE_PATH/src/ext \
        $$CLUCENE_SOURCE_PATH/src/shared \
        $$CLUCENE_SOURCE_PATH/src/contribs-lib \
        $$CLUCENE_BUILD_PATH/src/shared

LIBS += -lclucene-core$$CLUCENE_LIB_SUFFIX \
        -lclucene-shared$$CLUCENE_LIB_SUFFIX \
        -lclucene-contribs-lib$$CLUCENE_LIB_SUFFIX \
        -L$$CLUCENE_LIBS_PATH

unix {

    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    BINDIR = $$PREFIX/bin
    LIBDIR = $$PREFIX/lib
    DATADIR = $$PREFIX/share

    INSTALLS += target share data js locale styles

    target.path = $$BINDIR

    share.path = $$DATADIR
    share.files += ../share/fonts
    share.files += ../share/app-install
    share.files += ../share/applications
    share.files += ../share/pixmaps

    data.path = $$DATADIR/$$APPNAME
    data.files += ../share/$$APPNAME/data

    js.path = $$DATADIR/$$APPNAME
    js.files += ../share/$$APPNAME/js

    locale.path = $$DATADIR/$$APPNAME
    locale.files += ../share/$$APPNAME/locale

    styles.path = $$DATADIR/$$APPNAME
    styles.files += ../share/$$APPNAME/styles
}
