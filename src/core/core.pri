INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql xml network webkit

MOC_DIR += .moc
OBJECTS_DIR += .obj
UI_DIR += .ui
RCC_DIR += .rcc

SOURCES += abstarctview.cpp \
    abstractbookreader.cpp \
    arabicanalyzer.cpp \
    arabicfilter.cpp \
    arabictokenizer.cpp \
    authorsmanager.cpp \
    authorinfo.cpp \
    bookexception.cpp \
    bookexporter.cpp \
    bookindexerbase.cpp \
    booklistmanager.cpp \
    bookpage.cpp \
    bookutils.cpp \
    bookviewbase.cpp \
    clucenequery.cpp \
    clutils.cpp \
    cssformatter.cpp \
    databasemanager.cpp \
    fancylineedit.cpp \
    favouritesmanager.cpp \
    filterlineedit.cpp \
    htmlhelper.cpp \
    librarybook.cpp \
    librarybookmanager.cpp \
    libraryinfo.cpp \
    librarymanager.cpp \
    listmanager.cpp \
    importmodel.cpp \
    indextracker.cpp \
    indextaskiter.cpp \
    mimeutils.cpp \
    modelutils.cpp \
    qurantextformat.cpp \
    richbookreader.cpp \
    searchmanager.cpp \
    simpletextformat.cpp \
    sqlutils.cpp \
    statisticsmanager.cpp \
    stringutils.cpp \
    tafessirtextformat.cpp \
    taffesirlistmanager.cpp \
    tarajemrowatmanager.cpp \
    textformatter.cpp \
    timeutils.cpp \
    utils.cpp \
    uploader.cpp \
    webviewsearcher.cpp \
    wordtypefilter.cpp \
    xmldomhelper.cpp \
    xmlmanager.cpp \
    xmlutils.cpp \
    ziphelper.cpp \
    zipopener.cpp \
    ziputils.cpp

HEADERS += abstarctview.h \
    abstractbookreader.h \
    arabicanalyzer.h \
    arabicfilter.h \
    arabictokenizer.h \
    authorsmanager.h \
    authorinfo.h \
    bookexception.h \
    bookexporter.h \
    bookindexerbase.h \
    booklistmanager.h \
    bookpage.h \
    bookutils.h \
    bookviewbase.h \
    clconstants.h \
    clheader.h \
    clucenequery.h \
    clutils.h \
    cssformatter.h \
    databasemanager.h \
    fancylineedit.h \
    favouritesmanager.h \
    filterlineedit.h \
    htmlhelper.h \
    librarybook.h \
    librarybookmanager.h \
    libraryinfo.h \
    librarymanager.h \
    listmanager.h \
    importmodel.h \
    indextracker.h \
    indextaskiter.h \
    mimeutils.h \
    modelenums.h \
    modelutils.h \
    qurantextformat.h \
    richbookreader.h \
    searchmanager.h \
    simpletextformat.h \
    sqlutils.h \
    statisticsmanager.h \
    stringutils.h \
    tafessirtextformat.h \
    taffesirlistmanager.h \
    tarajemrowatmanager.h \
    textformatter.h \
    timeutils.h \
    utils.h \
    uploader.h \
    webviewsearcher.h \
    wordtypefilter.h \
    xmldomhelper.h \
    xmlmanager.h \
    xmlutils.h \
    ziphelper.h \
    zipopener.h \
    ziputils.h
