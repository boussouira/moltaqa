INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += QUAZIP_STATIC

exists(../../../zlib) {
    ZLIB_INCLUDE_PATH = ../../zlib/include
} else {
    ZLIB_INCLUDE_PATH = $$(ZLIB_INCLUDE)
}

INCLUDEPATH += $$ZLIB_INCLUDE_PATH

HEADERS += crypt.h \
           ioapi.h \
           JlCompress.h \
           quaadler32.h \
           quachecksum32.h \
           quacrc32.h \
           quazip.h \
           quagzipfile.h \
           quaziodevice.h \
           quazipdir.h \
           quazipfile.h \
           quazipfileinfo.h \
           quazipnewinfo.h \
           unzip.h \
           zip.h
SOURCES += qioapi.cpp \
           JlCompress.cpp \
           quaadler32.cpp \
           quacrc32.cpp \
           quagzipfile.cpp \
           quaziodevice.cpp \
           quazip.cpp \
           quazipdir.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c
