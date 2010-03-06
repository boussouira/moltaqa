# -------------------------------------------------
# Project created by QtCreator 2009-11-13T15:47:40
# -------------------------------------------------
QT += sql  webkit

TEMPLATE = app
TARGET = alkotobiya

MOC_DIR += build
OBJECTS_DIR += build
UI_DIR += build
RCC_DIR += build
DESTDIR += build

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/quransearch.cpp \
    src/sorainfo.cpp \
    src/ktext.cpp \
    src/ktab.cpp \
    src/quranmodel.cpp \
    src/ksetting.cpp
HEADERS += src/mainwindow.h \
    src/constant.h \
    src/quransearch.h \
    src/sorainfo.h \
    src/ktext.h \
    src/ktab.h \
    src/quranmodel.h \
    src/ksetting.h
FORMS += src/ui/mainwindow.ui \
    src/ui/quransearch.ui \
    src/ui/ksetting.ui
RESOURCES += res/qt_rc.qrc
win32:RC_FILE = res/win_rc.rc
