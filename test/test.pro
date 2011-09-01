TEMPLATE = app
TARGET = alkotobiya_test
QT += testlib sql

CONFIG += qtestlib

INCLUDEPATH += ../src

HEADERS += utilstest.h
SOURCES += utilstest.cpp ../src/utils.cpp
