lessThan(QT_VERSION, 4.7) {
    error("Moltaqa Library requires Qt 4.7 or greater")
}

TEMPLATE = subdirs
SUBDIRS  = src test
