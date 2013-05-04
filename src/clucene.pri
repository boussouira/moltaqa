
exists(../../clucene) {
    CLUCENE_SOURCE_PATH = $$PWD/../../clucene
    CLUCENE_BUILD_PATH = $$PWD/../../clucene

    CONFIG(debug, debug|release):exists($$PWD/../../clucene-debug) {
        CLUCENE_BUILD_PATH = $$PWD/../../clucene-debug
    }

    CONFIG(release, debug|release):exists($$PWD/../../clucene-release) {
        CLUCENE_BUILD_PATH = $$PWD/../../clucene-release
    }
} else {
    CLUCENE_SOURCE_PATH = $$(CLUCENE_SOURCE_PATH)
    CLUCENE_BUILD_PATH = $$(CLUCENE_BUILD_PATH)
}

!exists($$CLUCENE_SOURCE_PATH)|!exists($$CLUCENE_BUILD_PATH) {
    error(CLucene not found: [$$CLUCENE_SOURCE_PATH] [$$CLUCENE_BUILD_PATH])
} else {
    message(CLucene source: $$CLUCENE_SOURCE_PATH)
    message(CLucene build: $$CLUCENE_BUILD_PATH)
}

win32 {
    win32-msvc* {
        CONFIG(debug, debug|release) {
            CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin/debug
            CLUCENE_LIB_SUFFIX = "d"
        }

        CONFIG(release, debug|release) {
            CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin/release
        }

        DEFINES +=  _CRT_SECURE_NO_DEPRECATE \
                    _CRT_NONSTDC_NO_DEPRECATE
    }
}

unix {
    CLUCENE_LIBS_PATH = $$CLUCENE_BUILD_PATH/bin
}

DEFINES += _REENTRANT _UCS2 _UNICODE

INCLUDEPATH += $$CLUCENE_SOURCE_PATH/src/core \
        $$CLUCENE_SOURCE_PATH/src/ext \
        $$CLUCENE_SOURCE_PATH/src/shared \
        $$CLUCENE_SOURCE_PATH/src/contribs-lib \
        $$CLUCENE_BUILD_PATH/src/shared

LIBS += -lclucene-core$$CLUCENE_LIB_SUFFIX \
        -lclucene-shared$$CLUCENE_LIB_SUFFIX \
        -lclucene-contribs-lib$$CLUCENE_LIB_SUFFIX \
        -L$$CLUCENE_LIBS_PATH
