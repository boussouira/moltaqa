project(alkotobiya)

find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtSql QtNetwork QtXmlPatterns QtWebKit REQUIRED)
include(${QT_USE_FILE})

# Check for mdbtools
IF(NOT WIN32)
    FIND_PACKAGE(PkgConfig)
    PKG_CHECK_MODULES(LIBMDB libmdb)

    IF(LIBMDB_FOUND) #We got mdbtools
        ADD_DEFINITIONS(-DUSE_MDBTOOLS)
        include_directories(${LIBMDB_INCLUDE_DIRS})
        link_directories(${LIBMDB_LIBRARIES_DIRS})
    ELSE(LIBMDB_FOUND)
        MESSAGE(FATAL_ERROR "Could not find mdbtools.")
    ENDIF(LIBMDB_FOUND)

ENDIF(NOT WIN32)

include_directories( ${alkotobiya_SOURCE_DIR} )
include_directories( ${alkotobiya_BINARY_DIR} )

set(alkotobiya_SRCS
    abstractdbhandler.cpp
    bookindexmodel.cpp
    bookindexnode.cpp
    bookinfo.cpp
    booksindexdb.cpp
    bookslistbrowser.cpp
    bookslistmodel.cpp
    bookslistnode.cpp
    bookexception.cpp
    booksviewer.cpp
    bookwidget.cpp
    catslistwidget.cpp
    convertthread.cpp
    importdelegates.cpp
    importdialog.cpp
    importmodel.cpp
    importthread.cpp
    indexwidget.cpp
    webview.cpp
    main.cpp
    mainwindow.cpp
    pageinfo.cpp
    qurandbhandler.cpp
    qurantextformat.cpp
    settingschecker.cpp
    settingsdialog.cpp
    simpledbhandler.cpp
    simpletextformat.cpp
    tabwidget.cpp
    textformatter.cpp
)

IF(LIBMDB_FOUND)
    list(APPEND alkotobiya_SRCS mdbconverter_unix.cpp)
ELSE(LIBMDB_FOUND)
    list(APPEND alkotobiya_SRCS mdbconverter_win.cpp)
ENDIF(LIBMDB_FOUND)

file (GLOB alkotobiya_UIS *.ui)

set(alkotobiya_MOCS_SRCS
    catslistwidget.h
    convertthread.h
    importdelegates.h
    importdialog.h
    importthread.h
    booksviewer.h
    bookslistbrowser.h
    indexwidget.h
    bookwidget.h
    settingsdialog.h
    tabwidget.h
    webview.h
    mainwindow.h
)
set(alkotobiya_RCCS data/qt_rc.qrc)

qt4_wrap_cpp(alkotobiya_MOCS ${alkotobiya_MOCS_SRCS})
qt4_wrap_ui(alkotobiya_UIS_H ${alkotobiya_UIS})
qt4_add_resources(alkotobiya_RCC_SRCS ${alkotobiya_RCCS})

add_executable(alkotobiya
    ${alkotobiya_SRCS}
    ${alkotobiya_MOCS}
    ${alkotobiya_UIS}
    ${alkotobiya_RCC_SRCS})

target_link_libraries(alkotobiya
    ${QT_LIBRARIES}
    ${LIBMDB_LIBRARIES})
