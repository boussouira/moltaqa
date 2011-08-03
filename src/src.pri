INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql webkit xml

MOC_DIR += .moc
OBJECTS_DIR += .obj
UI_DIR += .ui
RCC_DIR += .rcc

unix {
        DEFINES += USE_MDBTOOLS
        HEADERS += mdbconverter.h
        SOURCES += mdbconverter.cpp

        QMAKE_CXXFLAGS += $$system(pkg-config libmdb --cflags)
        LIBS += $$system(pkg-config libmdb --libs)
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
    webview.cpp \
    settingschecker.cpp \
    textformatter.cpp \
    importdialog.cpp \
    importmodel.cpp \
    importdelegates.cpp \
    catslistwidget.cpp \
    convertthread.cpp \
    importthread.cpp \
    bookexception.cpp \
    libraryinfo.cpp \
    sqlitelibraryinfo.cpp \
    indexdb.cpp \
    sqliteindexdb.cpp \
    simplequery.cpp \
    sqlitesimplequery.cpp \
    quranquery.cpp \
    sqlitequranquery.cpp \
    welcomewidget.cpp \
    newbookwriter.cpp \
    common.cpp
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
    webview.h \
    settingschecker.h \
    textformatter.h \
    importdialog.h \
    importmodel.h \
    importdelegates.h \
    catslistwidget.h \
    convertthread.h \
    importthread.h \
    bookexception.h \
    libraryinfo.h \
    sqlitelibraryinfo.h \
    indexdb.h \
    sqliteindexdb.h \
    simplequery.h \
    sqlitesimplequery.h \
    quranquery.h \
    sqlitequranquery.h \
    welcomewidget.h \
    newbookwriter.h \
    common.h
FORMS += mainwindow.ui settingsdialog.ui \
    indexwidget.ui \
    bookslistbrowser.ui \
    importdialog.ui \
    welcomewidget.ui
RESOURCES += data/qt_rc.qrc
