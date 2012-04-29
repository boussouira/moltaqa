TEMPLATE = app
APPNAME = moltaqa-lib

win32 {
    TARGET = $$APPNAME
} else {
    TARGET = $$join(APPNAME, "","", ".bin")
}

CODECFORSRC = UTF-8
CODECFORTR  = UTF-8

include(src.pri)
include(quazip/quazip.pri)

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
        PREFIX = /usr/local
    }

    BINDIR = $$PREFIX/bin
    LIBDIR = $$PREFIX/lib
    DATADIR = $$PREFIX/share


    INSTALLS += target script data styles js locale fonts

    target.path = $$BINDIR

    script.path = $$BINDIR
    script.files += ../bin/moltaqa-lib

    data.path = $$DATADIR/$$APPNAME/data
    data.files += ../share/$$APPNAME/data/quran-meta.db

    styles.path = $$DATADIR/$$APPNAME/styles/default
    styles.files += ../share/$$APPNAME/styles/default/default.css

    js.path = $$DATADIR/$$APPNAME/js
    js.files += ../share/$$APPNAME/js/jquery.js
    js.files += ../share/$$APPNAME/js/jquery.growl.js
    js.files += ../share/$$APPNAME/js/jquery.pagination.js
    js.files += ../share/$$APPNAME/js/jquery.tooltip.js
    js.files += ../share/$$APPNAME/js/scripts.js
    js.files += ../share/$$APPNAME/js/editor.js
    js.files += ../share/$$APPNAME/js/reader.js
    js.files += ../share/$$APPNAME/js/search.js
    js.files += ../share/$$APPNAME/js/ckeditor

    locale.path = $$DATADIR/$$APPNAME/locale
    locale.files += ../share/$$APPNAME/locale/qt_ar.qm

    fonts.path = $$DATADIR/fonts/$$APPNAME
    fonts.files += ../share/fonts/$$APPNAME/LotusLinotype_Light.otf
    fonts.files += ../share/fonts/$$APPNAME/LotusLinotype_Bold.otf
}
