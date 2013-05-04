TEMPLATE = app
TARGET = moltaqa-lib_test
QT += testlib sql

CONFIG += qtestlib
DESTDIR = ../bin

INCLUDEPATH += ../src/core

HEADERS += utilstest.h
SOURCES += utilstest.cpp \
    ../src/core/utils.cpp \
    ../src/core/stringutils.cpp \
    ../src/core/sqlutils.cpp  \
    ../src/core/bookutils.cpp
