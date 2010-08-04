project(alkotobiya)

find_package(Qt4 4.5 COMPONENTS QtCore QtGui QtSql QtNetwork QtXmlPatterns QtWebKit REQUIRED)
include(${QT_USE_FILE})

file (GLOB alkotobiya_SRCS *.cpp)
file (GLOB alkotobiya_UIS *.ui)

set(alkotobiya_MOCS_SRCS
    booksviewer.h
    bookslistbrowser.h
    indexwidget.h
    bookwidget.h
    settingsdialog.h
    tabwidget.h
    kwebview.h
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

target_link_libraries(alkotobiya ${QT_LIBRARIES} pthread)

