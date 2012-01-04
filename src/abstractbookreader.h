#ifndef ABSTRACTBOOKREADER_H
#define ABSTRACTBOOKREADER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>
#include <qfile.h>
#include <qdom.h>

#include "librarymanager.h"
#include "sqlutils.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class LibraryInfo;
class LibraryBook;
class BookPage;
class BookIndexModel;
class TextFormatter;
class QSqlQuery;

class AbstractBookReader : public QObject
{

public:
    AbstractBookReader(QObject *parent=0);
    ~AbstractBookReader();
    void openBook();
    void setBookInfo(LibraryBook *bi);
    void setLibraryManager(LibraryManager *db);
    void setBookIndexModel(BookIndexModel *model);

    LibraryBook *bookInfo() { return m_bookInfo; }
    LibraryManager *libraryManager();
    BookPage *page();

    /**
      Open page with the given id

      @param pid page id, if pid == -1 then open the first page, if pid == -2 the open last page
      */
    virtual void goToPage(int pid = -1);

    /**
      Open page at the given page and part number
      */
    virtual void goToPage(int page, int part);

    /**
      Open first page with given sora and aya number
      */
    virtual void goToSora(int sora, int aya);

    /**
      Open page with the given haddit number
      */
    virtual void goToHaddit(int hadditNum);

    virtual void firstPage();
    virtual void lastPage();

    /**
      Open next page
      */
    virtual void nextPage();
    /**
      Open Previous page
      */
    virtual void prevPage();

    /**
      Go to next unit

      In quarn book, unit mean an AYA, so when calling this function on RichQuranReader while
      highlight the next aya if it exists in the currentpage, or it go to the next page and
      highlight it.
      */
    virtual void nextAya();

    /**
      Go to Previous unit
      */
    virtual void prevAya();

    /**
      Check if this reader can go to the next page
      @return True if it can go to the next page
      */
    virtual bool hasNext();
    /**
      Check if this reader can go to the previous page
      @return True if it can go to the previous page
      */
    virtual bool hasPrev();

    static bool getBookPage(LibraryBook *book, BookPage *page);
    static QString getFileContent(QuaZip *zip, QString fileName);

    inline QString getFileContent(QString fileName)
    {
        return getFileContent(&m_zip, fileName);
    }

    inline QString getPageText(int pageID)
    {
        return getFileContent(&m_zip, QString("pages/p%1.html").arg(pageID));
    }

    static QString getPageText(QuaZip *zip, int pageID)
    {
        return getFileContent(zip, QString("pages/p%1.html").arg(pageID));
    }


protected:
    /**
      Generate book info
      */
    virtual void getBookInfo();

    virtual void connected();

    virtual void setCurrentPage(QDomElement pageNode)=0;

    QDomElement getPage(int pid);
    QDomElement getPageId(int page, int part);
    QDomElement getPageId(int haddit);
    virtual QDomElement getQuranPageId(int sora, int aya);

private:
    static bool getSimpleBookPage(QuaZip *zip, LibraryBook *book, BookPage *page);
    static bool getTafessirPage(QuaZip *zip, LibraryBook *book, BookPage *page);
    static bool getQuranPage(QuaZip *zip, LibraryBook *book, BookPage *page);

protected:
    LibraryBook *m_bookInfo;
    BookPage *m_currentPage;
    LibraryManager *m_libraryManager;
    BookIndexModel *m_indexModel;
    QSqlDatabase m_bookDB;
    QString m_bookDBPath;
    QString m_connectionName;
    QFile m_zipFile;
    QuaZip m_zip;
    QuaZipFile m_pagesMetaFile;
    QDomDocument m_bookDoc;
    QDomElement m_rootElement;
    QDomElement m_currentElement;
};

#endif // ABSTRACTBOOKREADER_H
