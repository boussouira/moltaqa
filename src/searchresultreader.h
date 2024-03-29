#ifndef SEARCHRESULTREADER_H
#define SEARCHRESULTREADER_H

#include "librarybook.h"
#include "xmldomhelper.h"

#include <qcache.h>
#include <qobject.h>

class QuaZip;
class BookReaderHelper;

class SearchResultReader : public QObject
{
    Q_OBJECT
public:
    SearchResultReader(QObject *parent = 0);

    void setShowPageTitle(bool show);
    void setShowPageInfo(bool show);

    bool getBookPage(LibraryBook::Ptr book, BookPage *page);

protected:
    bool getSimpleBookPage(QuaZip *zip, LibraryBook::Ptr book, BookPage *page);
    bool getQuranPage(QuaZip *zip, LibraryBook::Ptr book, BookPage *page);

    bool getPageInfo(QuaZip *zip, LibraryBook::Ptr book, BookPage *page);
    bool getPageText(QuaZip *zip, BookPage *page);
    bool getPageTitle(QuaZip *zip, LibraryBook::Ptr book, BookPage *page);

    void taffesirTitle(BookPage *page);

protected:
    BookReaderHelper *m_readerHelper;
    bool m_showPageTitle;
    bool m_showPageInfo;
    bool m_hierarchyTitle;
};

#endif // SEARCHRESULTREADER_H
