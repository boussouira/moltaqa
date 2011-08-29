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
    textformatter.cpp \
    importdialog.cpp \
    importmodel.cpp \
    importdelegates.cpp \
    catslistwidget.cpp \
    convertthread.cpp \
    bookexception.cpp \
    libraryinfo.cpp \
    indexdb.cpp \
    simplequery.cpp \
    quranquery.cpp \
    welcomewidget.cpp \
    newbookwriter.cpp \
    common.cpp \
    shamelaimportdialog.cpp \
    shamelainfo.cpp \
    shamelamanager.cpp \
    shamelaimportthread.cpp \
    librarycreator.cpp \
    shamelaimportinfo.cpp \
    openpagedialog.cpp \
    newquranwriter.cpp \
    sortfilterproxymodel.cpp \
    newlibrarydialog.cpp \
    tafessirdbhandler.cpp \
    tafessirtextformat.cpp \
    tafessirquery.cpp \
    shamelamapper.cpp \
    controlcenterdialog.cpp \
    editablebookslistmodel.cpp \
    editcatwidget.cpp \
    selectcatdialog.cpp \
    editbookslistwidget.cpp \
    editablecatslistmodel.cpp \
    editindexdb.cpp \
    selectauthordialog.cpp \
    abstracteditwidget.cpp
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
    textformatter.h \
    importdialog.h \
    importmodel.h \
    importdelegates.h \
    catslistwidget.h \
    convertthread.h \
    bookexception.h \
    libraryinfo.h \
    indexdb.h \
    simplequery.h \
    quranquery.h \
    welcomewidget.h \
    newbookwriter.h \
    common.h \
    shamelaimportdialog.h \
    shamelainfo.h \
    shamelamanager.h \
    shamelaimportthread.h \
    librarycreator.h \
    shamelaimportinfo.h \
    openpagedialog.h \
    newquranwriter.h \
    sortfilterproxymodel.h \
    newlibrarydialog.h \
    tafessirdbhandler.h \
    tafessirtextformat.h \
    tafessirquery.h \
    shamelamapper.h \
    controlcenterdialog.h \
    editablebookslistmodel.h \
    editcatwidget.h \
    selectcatdialog.h \
    editbookslistwidget.h \
    editablecatslistmodel.h \
    editindexdb.h \
    selectauthordialog.h \
    abstracteditwidget.h
FORMS += mainwindow.ui settingsdialog.ui \
    indexwidget.ui \
    bookslistbrowser.ui \
    importdialog.ui \
    welcomewidget.ui \
    shamelaimportdialog.ui \
    openpagedialog.ui \
    newlibrarydialog.ui \
    controlcenterdialog.ui \
    editcatwidget.ui \
    selectcatdialog.ui \
    editbookslistwidget.ui \
    selectauthordialog.ui

RESOURCES += ../data/qt_rc.qrc
win32:RC_FILE = ../data/win_rc.rc
