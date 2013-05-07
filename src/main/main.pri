INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += ../core

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
        LIBS += /usr/lib/i386-linux-gnu/libmdb.a -lglib-2.0
}

SOURCES += aboutdialog.cpp \
    authorsmanagerwidget.cpp \
    authorsview.cpp \
    bookeditor.cpp \
    bookeditorview.cpp \
    bookexporterthread.cpp \
    bookfilesreader.cpp \
    bookhistorydialog.cpp \
    bookindexeditor.cpp \
    bookindexerquran.cpp \
    bookindexersimple.cpp \
    bookinfodialog.cpp \
    booklistmanagerwidget.cpp \
    bookmediaeditor.cpp \
    bookreaderhelper.cpp \
    bookreaderview.cpp \
    bookreferedialog.cpp \
    booksearchfilter.cpp \
    booksearchwidget.cpp \
    bookslistbrowser.cpp \
    bookwidget.cpp \
    bookwidgetmanager.cpp \
    checkablemessagebox.cpp \
    controlcenterdialog.cpp \
    controlcenterwidget.cpp \
    convertthread.cpp \
    editwebview.cpp \
    epubbookexporter.cpp \
    exportdialog.cpp \
    exportdialogpages.cpp \
    favouritesmanagerwidget.cpp \
    favouritessearchfilter.cpp \
    favouritessearchwidget.cpp \
    fancylineedit.cpp \
    filterlineedit.cpp \
    filechooserwidget.cpp \
    htmlbookexporter.cpp \
    importdelegates.cpp \
    importdialog.cpp \
    indexmanager.cpp \
    indexwidget.cpp \
    librarybookexporter.cpp \
    librarybookmanagerwidget.cpp \
    librarycreator.cpp \
    librarysearcher.cpp \
    librarysearchfilter.cpp \
    librarysearchwidget.cpp \
    logdialog.cpp \
    loghighlighter.cpp \
    mainwindow.cpp \
    modelviewfilter.cpp \
    modelviewsearcher.cpp \
    newbookwriter.cpp \
    newlibrarydialog.cpp \
    newquranwriter.cpp \
    openpagedialog.cpp \
    resultwidget.cpp \
    richquranreader.cpp \
    richsimplebookreader.cpp \
    richtafessirreader.cpp \
    searchfieldsdialog.cpp \
    searchfiltermanager.cpp \
    searchquerywidget.cpp \
    searchresult.cpp \
    searchresultreader.cpp \
    searchview.cpp \
    searchwidget.cpp \
    selectauthordialog.cpp \
    selectbooksdialog.cpp \
    selectcatdialog.cpp \
    settingsdialog.cpp \
    shamelaimportdialog.cpp \
    shamelaimportinfo.cpp \
    shamelaimportthread.cpp \
    shamelainfo.cpp \
    shamelamanager.cpp \
    shamelamapper.cpp \
    shemehandler.cpp \
    sortfilterproxymodel.cpp \
    tabwidget.cpp \
    taffesirlistmanagerwidget.cpp \
    tarajemrowatmanagerwidget.cpp \
    tarajemrowatview.cpp \
    updatechecker.cpp \
    updatedialog.cpp \
    viewmanager.cpp \
    webpage.cpp \
    webpagenam.cpp \
    webview.cpp \
    webviewsearcher.cpp \
    welcomewidget.cpp \
    windowsview.cpp \
    wizardpage.cpp \
    bookindexerthread.cpp

HEADERS += aboutdialog.h \
    authorsmanagerwidget.h \
    authorsview.h \
    bookeditor.h \
    bookeditorview.h \
    bookexporterthread.h \
    bookfilesreader.h \
    bookhistorydialog.h \
    bookindexeditor.h \
    bookindexerquran.h \
    bookindexersimple.h \
    bookinfodialog.h \
    booklistmanagerwidget.h \
    bookmediaeditor.h \
    bookreaderhelper.h \
    bookreaderview.h \
    bookreferedialog.h \
    booksearchfilter.h \
    booksearchwidget.h \
    bookslistbrowser.h \
    bookwidget.h \
    bookwidgetmanager.h \
    checkablemessagebox.h \
    controlcenterdialog.h \
    controlcenterwidget.h \
    convertthread.h \
    editwebview.h \
    epubbookexporter.h \
    exportdialog.h \
    exportdialogpages.h \
    exportformats.h \
    favouritesmanagerwidget.h \
    favouritessearchfilter.h \
    favouritessearchwidget.h \
    fancylineedit.h \
    filterlineedit.h \
    filechooserwidget.h \
    htmlbookexporter.h \
    importdelegates.h \
    importdialog.h \
    indexmanager.h \
    indexwidget.h \
    librarybookexporter.h \
    librarybookmanagerwidget.h \
    librarycreator.h \
    libraryenums.h \
    librarysearcher.h \
    librarysearchfilter.h \
    librarysearchwidget.h \
    logdialog.h \
    loghighlighter.h \
    mainwindow.h \
    modelviewfilter.h \
    modelviewsearcher.h \
    newbookwriter.h \
    newlibrarydialog.h \
    newquranwriter.h \
    openpagedialog.h \
    resultwidget.h \
    richquranreader.h \
    richsimplebookreader.h \
    richtafessirreader.h \
    searchfieldsdialog.h \
    searchfiltermanager.h \
    searchquerywidget.h \
    searchresult.h \
    searchresultreader.h \
    searchview.h \
    searchwidget.h \
    selectauthordialog.h \
    selectbooksdialog.h \
    selectcatdialog.h \
    settingsdialog.h \
    shamelaimportdialog.h \
    shamelaimportinfo.h \
    shamelaimportthread.h \
    shamelainfo.h \
    shamelamanager.h \
    shamelamapper.h \
    shemehandler.h \
    sortfilterproxymodel.h \
    tabwidget.h \
    taffesirlistmanagerwidget.h \
    tarajemrowatmanagerwidget.h \
    tarajemrowatview.h \
    updatechecker.h \
    updatedialog.h \
    viewmanager.h \
    webpage.h \
    webpagenam.h \
    webview.h \
    webviewsearcher.h \
    welcomewidget.h \
    windowsview.h \
    wizardpage.h \
    bookindexerthread.h

FORMS += aboutdialog.ui \
    addbookreferdialog.ui \
    authorsmanagerwidget.ui \
    authorsview.ui \
    bookeditorview.ui \
    bookhistorydialog.ui \
    bookindexeditor.ui \
    booklistmanagerwidget.ui \
    bookmediaeditor.ui \
    bookreferedialog.ui \
    bookslistbrowser.ui \
    controlcenterdialog.ui \
    favouritesmanagerwidget.ui \
    importdialog.ui \
    indexwidget.ui \
    librarybookmanagerwidget.ui \
    logdialog.ui \
    mainwindow.ui \
    newlibrarydialog.ui \
    openpagedialog.ui \
    resultwidget.ui \
    searchfieldsdialog.ui \
    searchquerywidget.ui \
    searchwidget.ui \
    selectauthordialog.ui \
    selectbooksdialog.ui \
    selectcatdialog.ui \
    settingsdialog.ui \
    shamelaimportdialog.ui \
    taffesirlistmanagerwidget.ui \
    tarajemrowatmanagerwidget.ui \
    tarajemrowatview.ui \
    updatedialog.ui

RESOURCES += ../../share/moltaqa-lib/qt_rc.qrc
win32:RC_FILE = ../../share/moltaqa-lib/win_rc.rc
