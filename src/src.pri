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
    bookpage.cpp \
    qurantextformat.cpp \
    tabwidget.cpp \
    settingsdialog.cpp \
    indexwidget.cpp \
    booksviewer.cpp \
    bookslistbrowser.cpp \
    abstractbookreader.cpp \
    richquranreader.cpp \
    richsimplebookreader.cpp \
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
    shamelamapper.cpp \
    controlcenterdialog.cpp \
    booklistmanagerwidget.cpp \
    selectcatdialog.cpp \
    librarybookmanagerwidget.cpp \
    selectauthordialog.cpp \
    controlcenterwidget.cpp \
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
    librarysearchwidget.cpp \
    fancylineedit.cpp \
    cssformatter.cpp \
    searchfiltermanager.cpp \
    librarysearcher.cpp \
    searchresult.cpp \
    resultwidget.cpp \
    textsimplebookreader.cpp \
    textquranreader.cpp \
    bookreaderhelper.cpp \
    sqlutils.cpp \
    taffesirlistmanagerwidget.cpp \
    searchwidget.cpp \
    booksearchwidget.cpp \
    librarysearchfilter.cpp \
    booksearchfilter.cpp \
    texttafessirreader.cpp \
    wordtypefilter.cpp \
    bookeditorview.cpp \
    editwebview.cpp \
    bookeditor.cpp \
    htmlhelper.cpp \
    bookindexeditor.cpp \
    modelviewfilter.cpp \
    modelutils.cpp \
    xmlutils.cpp \
    taffesirlistmanager.cpp \
    listmanager.cpp \
    booklistmanager.cpp \
    librarybookmanager.cpp \
    authorsmanager.cpp \
    authorsmanagerwidget.cpp \
    xmldomhelper.cpp \
    zipopener.cpp \
    textbookindexer.cpp \
    simplebookindexer.cpp \
    quranbookindexer.cpp \
    stringutils.cpp \
    clucenequery.cpp \
    webpage.cpp \
    tarajemrowatview.cpp \
    tarajemrowatmanager.cpp \
    ziphelper.cpp \
    tarajemrowatmanagerwidget.cpp \
    xmlmanager.cpp \
    databasemanager.cpp \
    authorsview.cpp \
    shemehandler.cpp \
    favouritesmanager.cpp \
    favouritesmanagerwidget.cpp \
    timeutils.cpp \
    bookinfodialog.cpp \
    searchresultreader.cpp \
    aboutdialog.cpp \
    bookhistorydialog.cpp \
    searchmanager.cpp \
    searchfieldsdialog.cpp \
    bookreferedialog.cpp \
    selectbooksdialog.cpp \
    loghighlighter.cpp \
    logdialog.cpp \
    ziputils.cpp \
    favouritessearchwidget.cpp \
    favouritessearchfilter.cpp \
    filterlineedit.cpp \
    webviewsearcher.cpp
HEADERS += mainwindow.h \
    bookpage.h \
    qurantextformat.h \
    tabwidget.h \
    settingsdialog.h \
    indexwidget.h \
    booksviewer.h \
    bookslistbrowser.h \
    abstractbookreader.h \
    richquranreader.h \
    richsimplebookreader.h \
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
    shamelamapper.h \
    controlcenterdialog.h \
    booklistmanagerwidget.h \
    selectcatdialog.h \
    librarybookmanagerwidget.h \
    selectauthordialog.h \
    controlcenterwidget.h \
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
    librarysearchwidget.h \
    fancylineedit.h \
    cssformatter.h \
    searchfiltermanager.h \
    modelenums.h \
    librarysearcher.h \
    searchresult.h \
    resultwidget.h \
    textsimplebookreader.h \
    textquranreader.h \
    bookreaderhelper.h \
    sqlutils.h \
    libraryenums.h \
    taffesirlistmanagerwidget.h \
    searchwidget.h \
    booksearchwidget.h \
    librarysearchfilter.h \
    booksearchfilter.h \
    texttafessirreader.h \
    wordtypefilter.h \
    bookeditorview.h \
    editwebview.h \
    bookeditor.h \
    htmlhelper.h \
    bookindexeditor.h \
    modelviewfilter.h \
    modelutils.h \
    xmlutils.h \
    taffesirlistmanager.h \
    listmanager.h \
    booklistmanager.h \
    librarybookmanager.h \
    authorsmanager.h \
    authorsmanagerwidget.h \
    xmldomhelper.h \
    zipopener.h \
    textbookindexer.h \
    simplebookindexer.h \
    quranbookindexer.h \
    stringutils.h \
    clucenequery.h \
    webpage.h \
    tarajemrowatview.h \
    tarajemrowatmanager.h \
    ziphelper.h \
    tarajemrowatmanagerwidget.h \
    xmlmanager.h \
    databasemanager.h \
    authorsview.h \
    shemehandler.h \
    favouritesmanager.h \
    favouritesmanagerwidget.h \
    timeutils.h \
    bookinfodialog.h \
    searchresultreader.h \
    aboutdialog.h \
    bookhistorydialog.h \
    searchmanager.h \
    searchfieldsdialog.h \
    bookreferedialog.h \
    selectbooksdialog.h \
    loghighlighter.h \
    logdialog.h \
    ziputils.h \
    favouritessearchwidget.h \
    favouritessearchfilter.h \
    filterlineedit.h \
    webviewsearcher.h
FORMS += mainwindow.ui settingsdialog.ui \
    indexwidget.ui \
    bookslistbrowser.ui \
    importdialog.ui \
    welcomewidget.ui \
    shamelaimportdialog.ui \
    openpagedialog.ui \
    newlibrarydialog.ui \
    controlcenterdialog.ui \
    booklistmanagerwidget.ui \
    selectcatdialog.ui \
    librarybookmanagerwidget.ui \
    selectauthordialog.ui \
    searchwidget.ui \
    resultwidget.ui \
    taffesirlistmanagerwidget.ui \
    bookeditorview.ui \
    bookindexeditor.ui \
    authorsmanagerwidget.ui \
    tarajemrowatview.ui \
    tarajemrowatmanagerwidget.ui \
    authorsview.ui \
    favouritesmanagerwidget.ui \
    aboutdialog.ui \
    bookhistorydialog.ui \
    searchfieldsdialog.ui \
    bookreferedialog.ui \
    addbookreferdialog.ui \
    selectbooksdialog.ui \
    logdialog.ui

RESOURCES += ../share/moltaqa-lib/qt_rc.qrc
win32:RC_FILE = ../share/moltaqa-lib/win_rc.rc
