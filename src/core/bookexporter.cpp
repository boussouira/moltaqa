#include "bookexporter.h"

BookExporter::BookExporter(QObject *parent) : QObject(parent)
{
    m_exportedBooks = 0;
    m_stop = false;
    m_exportInOnePackage = false;
    m_removeTashkil = false;
    m_addPageNumber = false;
}

