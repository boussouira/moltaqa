INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += QUAZIP_STATIC

HEADERS += crypt.h \
           ioapi.h \
           JlCompress.h \
           quaadler32.h \
           quachecksum32.h \
           quacrc32.h \
           quazip.h \
           quazipfile.h \
           quazipfileinfo.h \
           quazipnewinfo.h \
           unzip.h \
           zip.h
SOURCES += qioapi.cpp \
           JlCompress.cpp \
           quaadler32.cpp \
           quacrc32.cpp \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c
