#include "bookexporter.h"
#include "librarymanager.h"
#include "libraryinfo.h"

BookExporter::BookExporter(QObject *parent) : QObject(parent)
{
    m_tempDir = LibraryManager::instance()->libraryInfo()->tempDir();
    m_exportedBooks = 0;
    m_stop = false;
    m_exportInOnePackage = false;
    m_removeTashkil = false;
    m_addPageNumber = false;
}

