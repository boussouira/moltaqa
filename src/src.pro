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
    DATADIR = $$PREFIX/share

    INSTALLS += target styles locale fonts

    target.path = $$BINDIR

    styles.path = $$DATADIR/$$TARGET/styles/default
    styles.files += ../share/alkotobiya/styles/default/default.css

    locale.path = $$DATADIR/$$TARGET/locale
    locale.files += ../share/alkotobiya/locale/qt_ar.qm

    fonts.path = $$DATADIR/fonts/$$TARGET
    fonts.files += ../share/fonts/$$TARGET/LotusLinotype_Light.otf
    fonts.files += ../share/fonts/$$TARGET/LotusLinotype_Bold.otf
}

