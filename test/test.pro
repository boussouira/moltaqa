TEMPLATE = app
TARGET = moltaqa-lib_test
QT += testlib sql

CONFIG += qtestlib
DESTDIR = ../bin

INCLUDEPATH += ../src

HEADERS += utilstest.h
SOURCES += utilstest.cpp ../src/utils.cpp ../src/stringutils.cpp
