#ifndef ABSTRACTBOOKREADER_H
#define ABSTRACTBOOKREADER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>
#include "librarymanager.h"

class LibraryInfo;
class BookInfo;
class PageInfo;
class BookIndexModel;
class TextFormatter;
class QAbstractItemModel;
class QSqlQuery;

class AbstractBookReader : public QObject
{

public:
    AbstractBookReader(QObject *parent=0);
    ~AbstractBookReader();
    void openBookDB();
    void setBookInfo(BookInfo *bi);
    void setConnctionInfo(LibraryInfo *info);
    void setLibraryManager(LibraryManager *db);

    BookInfo *bookInfo() { return m_bookInfo; }
    LibraryManager *libraryManager();

    /**
      Open page with the given id

      @param pid page id, if pid == -1 then open the first page, if pid == -2 the open last page
      */
    virtual void goToPage(int pid = -1)=0;

    /**
      Open page at the given page and part number
      */
    virtual void goToPage(int page, int part)=0;

    /**
      Open first page with given sora and aya number
      */
    virtual void goToSora(int sora, int aya)=0;

    /**
      Open page with the given haddit number
      */
    virtual void goToHaddit(int hadditNum)=0;

    /**
      Open next page
      */
    virtual void nextPage() = 0;
    /**
      Open Previous page
      */
    virtual void prevPage() = 0;

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
      Check if this handler can go to the next page
      @return True if it can go to the next page
      */
    virtual bool hasNext() = 0;
    /**
      Check if this handler can go to the previous page
      @return True if it can go to the previous page
      */
    virtual bool hasPrev() = 0;

    /**
      Get current page text
      */
    virtual QString text() = 0;

protected:
    /**
      Generate book info
      */
    virtual void getBookInfo() = 0;

    virtual void connected();

protected:
    LibraryInfo *m_connetionInfo;
    BookInfo *m_bookInfo;
    PageInfo *m_currentPage;
    LibraryManager *m_libraryManager;
    BookIndexModel *m_indexModel;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_bookDBPath;
    QString m_connectionName;
};

#endif // ABSTRACTBOOKREADER_H
