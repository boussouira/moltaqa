#ifndef SEARCHRESULTREADER_H
#define SEARCHRESULTREADER_H

#include <qobject.h>
#include <qcache.h>
#include <qsharedpointer.h>
#include "librarybook.h"
#include "xmldomhelper.h"

class QuaZip;
class BookReaderHelper;

typedef QSharedPointer<XmlDomHelper> XmlDomHelperPtr;

class SearchResultReader : public QObject
{
    Q_OBJECT
public:
    SearchResultReader(QObject *parent = 0);

    void setShowPageTitle(bool show);
    void setShowPageInfo(bool show);

    bool getBookPage(LibraryBookPtr book, BookPage *page);

protected:
    bool getSimpleBookPage(QuaZip *zip, LibraryBookPtr book, BookPage *page);
    bool getQuranPage(QuaZip *zip, LibraryBookPtr book, BookPage *page);

    bool getPageInfo(QuaZip *zip, LibraryBookPtr book, BookPage *page);
    bool getPageText(QuaZip *zip, BookPage *page);
    bool getPageTitle(QuaZip *zip, LibraryBookPtr book, BookPage *page);

    void taffesirTitle(BookPage *page);

protected:
    BookReaderHelper *m_readerHelper;
    QCache<int, XmlDomHelper> m_pagesDom;
    QCache<int, XmlDomHelper> m_titlesDom;
    bool m_showPageTitle;
    bool m_showPageInfo;
};

#endif // SEARCHRESULTREADER_H
