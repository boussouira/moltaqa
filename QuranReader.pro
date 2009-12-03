# -------------------------------------------------
# Project created by QtCreator 2009-11-13T15:47:40
# -------------------------------------------------
QT += sql
TARGET = QuranReader
TEMPLATE = app
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/ctextcrusor.cpp \
    src/quransearch.cpp \
    src/sorainfo.cpp \
    src/settings.cpp
HEADERS += src/mainwindow.h \
    src/ctextcrusor.h \
    src/constant.h \
    src/quransearch.h \
    src/sorainfo.h \
    src/settings.h
FORMS += ui/mainwindow.ui \
    ui/quransearch.ui
RESOURCES += resources/QResource.qrc
win32:RC_FILE = resources/QuranReader.rc
