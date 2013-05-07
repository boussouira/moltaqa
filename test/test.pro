TEMPLATE = app
TARGET = moltaqa-lib_test
QT += testlib sql

CONFIG += qtestlib
DESTDIR = ../bin

INCLUDEPATH += ../src/core
LIBS += -L../src/core -lmoltaqa-core

HEADERS += utilstest.h
SOURCES += utilstest.cpp
