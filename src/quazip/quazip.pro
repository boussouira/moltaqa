TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += QUAZIP_BUILD
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

# Input
HEADERS += \
    crypt.h\
    ioapi.h\
    JlCompress.h\
    quaadler32.h\
    quachecksum32.h\
    quacrc32.h\
    quazip.h\
    quazipfile.h\
    quazipfileinfo.h\
    quazipnewinfo.h\
    quazip_global.h\
    unzip.h\
    zip.h\

SOURCES += *.c *.cpp

unix:!symbian {
	if(empty($$PREFIX)) {
		PREFIX = /usr/local
	}

    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

	OBJECTS_DIR=.obj
	MOC_DIR=.moc
	
	LIBS += -lz
}

win32 {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target

    *-g++*: LIBS += -lz.dll
    *-msvc*: LIBS += -lzlibwapi
    *-msvc*: QMAKE_LFLAGS += /IMPLIB:$$DESTDIR\\quazip.lib
}


symbian {

    # Note, on Symbian you may run into troubles with LGPL.
    # The point is, if your application uses some version of QuaZip,
    # and a newer binary compatible version of QuaZip is released, then
    # the users of your application must be able to relink it with the
    # new QuaZip version. For example, to take advantage of some QuaZip
    # bug fixes.

    # This is probably best achieved by building QuaZip as a static
    # library and providing linkable object files of your application,
    # so users can relink it.

    CONFIG += staticlib
    CONFIG += debug_and_release

    LIBS += -lezip

    #Export headers to SDK Epoc32/include directory
    exportheaders.sources = $$HEADERS
    exportheaders.path = quazip
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$exportheaders.path/$$basename(header)"
    }
}
