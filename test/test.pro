TEMPLATE = app
TARGET = alkotobiya_test
QT += testlib sql

CONFIG += qtestlib
DESTDIR = ../bin

INCLUDEPATH += ../src

HEADERS += utilstest.h
SOURCES += utilstest.cpp ../src/utils.cpp
