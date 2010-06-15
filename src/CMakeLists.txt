project(alkotobiya)

find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtSql QtNetwork QtXmlPatterns QtWebKit REQUIRED)
include(${QT_USE_FILE})
include_directories()
link_directories ()
set(alkotobiya_SRCS
	booksviewer.cpp
	bookslistbrowser.cpp
	bookslistmodel.cpp
	bookslistnode.cpp
	abstractdbhandler.cpp
	indexwidget.cpp
	ksetting.cpp
	ktab.cpp
	main.cpp
	mainwindow.cpp
	pageinfo.cpp
	quransearch.cpp
	qurantextbase.cpp
	qurantextformat.cpp
	qurantextmodel.cpp
	simpledbhandler.cpp
	tafessirtextbase.cpp
	tafessirtextformat.cpp
)
set(alkotobiya_MOCS_SRCS
	booksviewer.h
	bookslistbrowser.h
	indexwidget.h
	ksetting.h
	ktab.h
	mainwindow.h
	quransearch.h
)
set(alkotobiya_UIS
	bookslistbrowser.ui
	indexwidget.ui
	ksetting.ui
	mainwindow.ui
	quransearch.ui
)
set(alkotobiya_RCCS
    data/qt_rc.qrc
)
qt4_wrap_cpp(alkotobiya_MOCS ${alkotobiya_MOCS_SRCS})
qt4_wrap_ui(alkotobiya_UIS_H ${alkotobiya_UIS})
#qt4_wrap_cpp(alkotobiya_MOC_UI ${alkotobiya_UIS_H})
qt4_add_resources(alkotobiya_RCC_SRCS ${alkotobiya_RCCS})
add_definitions(-DQT_NO_DEBUG)
add_executable(alkotobiya
    ${alkotobiya_SRCS}
    ${alkotobiya_MOCS}
#   ${alkotobiya_MOC_UI}
    ${alkotobiya_UIS}
    ${alkotobiya_RCC_SRCS})
set (EXECUTABLE_OUTPUT_PATH ../ )
target_link_libraries(alkotobiya ${QT_LIBRARIES} pthread)
