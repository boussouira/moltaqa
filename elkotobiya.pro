# -------------------------------------------------
# Project created by QtCreator 2009-11-13T15:47:40
# -------------------------------------------------
QT += sql \
    webkit
TARGET = elkotobiya
TEMPLATE = app

#MOC_DIR += build
#OBJECTS_DIR += build
#UI_DIR += build

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/quransearch.cpp \
    src/sorainfo.cpp \
    src/settings.cpp \
    src/ktext.cpp
HEADERS += src/mainwindow.h \
    src/constant.h \
    src/quransearch.h \
    src/sorainfo.h \
    src/settings.h \
    src/ktext.h \
FORMS += src/ui/mainwindow.ui \
    src/ui/quransearch.ui
RESOURCES += resources/QResource.qrc
win32:RC_FILE = resources/elkotobiya.rc
