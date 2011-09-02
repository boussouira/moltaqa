TEMPLATE = app
TARGET = alkotobiya

CODECFORSRC = UTF-8
CODECFORTR  = UTF-8

include(src.pri)

SOURCES += main.cpp
DESTDIR = ../bin

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/$$TARGET

    INSTALLS += target styles locale

    target.path = $$BINDIR

    styles.path = $$DATADIR/styles/default
    styles.files += ../share/alkotobiya/styles/default/default.css

    locale.path = $$DATADIR/locale
    locale.files += ../share/alkotobiya/locale/qt_ar.qm
}
