#ifndef ABSTRACTBOOKREADER_H
#define ABSTRACTBOOKREADER_H

#include <qobject.h>
#include <qcoreapplication.h>
#include <qfile.h>
#include <qdom.h>

#include "librarymanager.h"
#include "sqlutils.h"
#include "xmldomhelper.h"
#include "zipopener.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class LibraryInfo;
class LibraryBook;
class BookPage;
class QStandardItemModel;
class TextFormatter;
class QSqlQuery;
class BookEditor;

class AbstractBookReader : public QObject
{

public:
    AbstractBookReader(QObject *parent=0);
    ~AbstractBookReader();

    void openBook();
    void setBookInfo(LibraryBookPtr bi);

    LibraryBookPtr bookInfo();
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

    int pagesCount();

    inline XmlDomHelper& pagesDom() { return m_pagesDom; }

    static QString getFileContent(QuaZip *zip, QString fileName);

    static QString getPageText(QuaZip *zip, int pageID)
    {
        return getFileContent(zip, QString("pages/p%1.html").arg(pageID));
    }

    QString getFileContent(QString fileName)
    {
        ZipOpener opener(this);
        return getFileContent(&m_zip, fileName);
    }

    QString getPageText(int pageID)
    {
        ZipOpener opener(this);
        return getFileContent(&m_zip, QString("pages/p%1.html").arg(pageID));
    }


protected:

    void openZip();
    void closeZip();

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

protected:
    friend class ZipOpener;
    LibraryBookPtr m_bookInfo;
    BookPage *m_currentPage;
    LibraryManager *m_libraryManager;
    QStandardItemModel *m_indexModel;
    QString m_bookDBPath;
    QString m_connectionName;
    QuaZip m_zip;
    XmlDomHelper m_pagesDom;
};

#endif // ABSTRACTBOOKREADER_H
