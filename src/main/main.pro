TEMPLATE = app
APPNAME = moltaqa-lib

TARGET = $$APPNAME

CODECFORSRC = UTF-8
CODECFORTR  = UTF-8

include(main.pri)
include(../global.pri)
include(../clucene.pri)
include(../quazip/quazip.pri)
include(../qtsingleapplication/qtsingleapplication.pri)

SOURCES += main.cpp
DESTDIR = ../../bin

LIBS += -L../core -lmoltaqa-core

win32:LIBS += -luser32

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
