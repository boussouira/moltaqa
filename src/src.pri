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
    quransearch.cpp \
    pageinfo.cpp \
    qurantextformat.cpp \
    ktab.cpp \
    ksetting.cpp \
    qurantextbase.cpp \
    qurantextmodel.cpp \
    tafessirtextbase.cpp \
    tafessirtextformat.cpp
HEADERS += mainwindow.h \
    constant.h \
    quransearch.h \
    pageinfo.h \
    qurantextformat.h \
    ktab.h \
    ksetting.h \
    qurantextbase.h \
    qurantextmodel.h \
    tafessirtextbase.h \
    tafessirtextformat.h
FORMS += mainwindow.ui quransearch.ui ksetting.ui
RESOURCES += data/qt_rc.qrc
win32:RC_FILE = data/win_rc.rc
