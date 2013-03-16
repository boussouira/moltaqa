lessThan(QT_VERSION, 4.8) {
    error("Moltaqa Library requires Qt 4.8 or greater")
}

TEMPLATE = subdirs
SUBDIRS  = src test
