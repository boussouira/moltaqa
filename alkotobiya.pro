lessThan(QT_VERSION, 4.5) {
    error("Al Kotobiya requires Qt 4.5 or greater")
}

TEMPLATE = subdirs
SUBDIRS  = src


