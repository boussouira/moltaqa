INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql webkit

MOC_DIR += $$PWD/.moc
OBJECTS_DIR += $$PWD/.obj
UI_DIR += $$PWD/.ui
RCC_DIR += $$PWD/.rcc

#exists(../.git/HEAD) {
#    GITVERSION=$$system(git log -n1 --pretty=format:%h)
#    !isEmpty(GITVERSION) {
#        GITCHANGENUMBER=$$system(git log --pretty=format:%h | wc -l)
#        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
#        DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
#    }
#}

SOURCES += mainwindow.cpp \
    pageinfo.cpp \
    qurantextformat.cpp \
    ktab.cpp \
    ksetting.cpp \
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
    bookinfohandler.cpp \
    bookwidget.cpp \
    kwebview.cpp
HEADERS += mainwindow.h \
    pageinfo.h \
    qurantextformat.h \
    ktab.h \
    ksetting.h \
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
    bookinfohandler.h \
    bookwidget.h \
    kwebview.h
FORMS += mainwindow.ui ksetting.ui \
    indexwidget.ui \
    bookslistbrowser.ui
RESOURCES += data/qt_rc.qrc
