INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql webkit xml

MOC_DIR += .moc
OBJECTS_DIR += .obj
UI_DIR += .ui
RCC_DIR += .rcc

unix {
        DEFINES += USE_MDBTOOLS
        HEADERS += mdbconverter.h mdbconvertermanager.h
        SOURCES += mdbconverter.cpp mdbconvertermanager.cpp

        QMAKE_CXXFLAGS += $$system(pkg-config libmdb --cflags)
        LIBS += $$system(pkg-config libmdb --libs)
}

SOURCES += mainwindow.cpp \
    bookpage.cpp \
    qurantextformat.cpp \
    tabwidget.cpp \
    settingsdialog.cpp \
    indexwidget.cpp \
    booksviewer.cpp \
    bookslistbrowser.cpp \
    bookslistmodel.cpp \
    bookslistnode.cpp \
    abstractbookreader.cpp \
    richquranreader.cpp \
    richsimplebookreader.cpp \
    bookindexmodel.cpp \
    bookindexnode.cpp \
    librarybook.cpp \
    simpletextformat.cpp \
    bookwidget.cpp \
    webview.cpp \
    textformatter.cpp \
    importdialog.cpp \
    importmodel.cpp \
    importdelegates.cpp \
    convertthread.cpp \
    bookexception.cpp \
    libraryinfo.cpp \
    librarymanager.cpp \
    simplequery.cpp \
    quranquery.cpp \
    welcomewidget.cpp \
    newbookwriter.cpp \
    utils.cpp \
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
    richtafessirreader.cpp \
    tafessirtextformat.cpp \
    tafessirquery.cpp \
    shamelamapper.cpp \
    controlcenterdialog.cpp \
    editablebookslistmodel.cpp \
    editcatwidget.cpp \
    selectcatdialog.cpp \
    editbookslistwidget.cpp \
    editablecatslistmodel.cpp \
    selectauthordialog.cpp \
    abstracteditwidget.cpp \
    bookwidgetmanager.cpp \
    richbookreader.cpp \
    indextracker.cpp \
    indextaskiter.cpp \
    textbookreader.cpp \
    indexmanager.cpp \
    bookindexer.cpp \
    arabicanalyzer.cpp \
    arabicfilter.cpp \
    arabictokenizer.cpp \
    clutils.cpp \
    abstarctview.cpp \
    viewmanager.cpp \
    searchview.cpp \
    searchwidget.cpp \
    fancylineedit.cpp \
    cssformatter.cpp \
    searchfiltermanager.cpp \
    librarysearcher.cpp \
    searchresult.cpp \
    resultwidget.cpp
HEADERS += mainwindow.h \
    bookpage.h \
    qurantextformat.h \
    tabwidget.h \
    settingsdialog.h \
    indexwidget.h \
    booksviewer.h \
    bookslistbrowser.h \
    bookslistmodel.h \
    bookslistnode.h \
    abstractbookreader.h \
    richquranreader.h \
    richsimplebookreader.h \
    bookindexmodel.h \
    bookindexnode.h \
    librarybook.h \
    simpletextformat.h \
    bookwidget.h \
    webview.h \
    textformatter.h \
    importdialog.h \
    importmodel.h \
    importdelegates.h \
    convertthread.h \
    bookexception.h \
    libraryinfo.h \
    librarymanager.h \
    simplequery.h \
    quranquery.h \
    welcomewidget.h \
    newbookwriter.h \
    utils.h \
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
    richtafessirreader.h \
    tafessirtextformat.h \
    tafessirquery.h \
    shamelamapper.h \
    controlcenterdialog.h \
    editablebookslistmodel.h \
    editcatwidget.h \
    selectcatdialog.h \
    editbookslistwidget.h \
    editablecatslistmodel.h \
    selectauthordialog.h \
    abstracteditwidget.h \
    bookwidgetmanager.h \
    richbookreader.h \
    indextracker.h \
    indextaskiter.h \
    textbookreader.h \
    indexmanager.h \
    bookindexer.h \
    clheader.h \
    arabicanalyzer.h \
    arabicfilter.h \
    arabictokenizer.h \
    clconstants.h \
    clutils.h \
    abstarctview.h \
    viewmanager.h \
    searchview.h \
    searchwidget.h \
    fancylineedit.h \
    cssformatter.h \
    searchfiltermanager.h \
    modelenums.h \
    librarysearcher.h \
    searchresult.h \
    resultwidget.h
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
    selectauthordialog.ui \
    searchwidget.ui \
    resultwidget.ui

RESOURCES += ../share/alkotobiya/qt_rc.qrc
win32:RC_FILE = ../share/alkotobiya/win_rc.rc
