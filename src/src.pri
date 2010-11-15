INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql webkit

MOC_DIR += .moc
OBJECTS_DIR += .obj
UI_DIR += .ui
RCC_DIR += .rcc

#exists(../.git/HEAD) {
#    GITVERSION=$$system(git log -n1 --pretty=format:%h)
#    !isEmpty(GITVERSION) {
#        GITCHANGENUMBER=$$system(git log --pretty=format:%h | wc -l)
#        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
#        DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
#    }
#}

unix {
        DEFINES += USE_MDBTOOLS
        HEADERS += mdbconverter_unix.h
        SOURCES += mdbconverter_unix.cpp

        QMAKE_CXXFLAGS += $$system(pkg-config libmdb --cflags)
        QMAKE_LIBS += $$system(pkg-config libmdb --libs)
}

win32 {
        HEADERS += mdbconverter_win.h
        SOURCES += mdbconverter_win.cpp
}

SOURCES += mainwindow.cpp \
    pageinfo.cpp \
    qurantextformat.cpp \
    tabwidget.cpp \
    settingsdialog.cpp \
    indexwidget.cpp \
    booksviewer.cpp \
    bookslistbrowser.cpp \
    bookslistmodel.cpp \
    bookslistnode.cpp \
    abstractdbhandler.cpp \
    qurandbhandler.cpp \
    simpledbhandler.cpp \
    bookindexmodel.cpp \
    bookindexnode.cpp \
    bookinfo.cpp \
    simpletextformat.cpp \
    bookwidget.cpp \
    kwebview.cpp \
    settingschecker.cpp \
    textformatter.cpp \
    importdialog.cpp \
    importmodel.cpp \
    importdelegates.cpp \
    booksindexdb.cpp \
    catslistwidget.cpp \
    convertthread.cpp \
    importthread.cpp \
    bookexception.cpp
HEADERS += mainwindow.h \
    pageinfo.h \
    qurantextformat.h \
    tabwidget.h \
    settingsdialog.h \
    indexwidget.h \
    booksviewer.h \
    bookslistbrowser.h \
    bookslistmodel.h \
    bookslistnode.h \
    abstractdbhandler.h \
    qurandbhandler.h \
    simpledbhandler.h \
    bookindexmodel.h \
    bookindexnode.h \
    bookinfo.h \
    simpletextformat.h \
    bookwidget.h \
    kwebview.h \
    settingschecker.h \
    textformatter.h \
    importdialog.h \
    importmodel.h \
    importdelegates.h \
    booksindexdb.h \
    catslistwidget.h \
    mdbconverter.h \
    convertthread.h \
    importthread.h \
    bookexception.h
FORMS += mainwindow.ui settingsdialog.ui \
    indexwidget.ui \
    bookslistbrowser.ui \
    importdialog.ui
RESOURCES += data/qt_rc.qrc
