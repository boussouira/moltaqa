#ifndef ABSTRACTDBHANDLER_H
#define ABSTRACTDBHANDLER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>
#include "indexdb.h"

class LibraryInfo;
class BookInfo;
class BookIndexModel;
class TextFormatter;
class QAbstractItemModel;
class QSqlQuery;

class AbstractDBHandler
{
    Q_DECLARE_TR_FUNCTIONS(AbstractDBHandler);

public:
    AbstractDBHandler();
    virtual ~AbstractDBHandler();
    void openBookDB(QString pBookDBPath=QString());
    void setBookInfo(BookInfo *bi);
    void setConnctionInfo(LibraryInfo *info);
    void setIndexDB(IndexDB *db);

    BookInfo *bookInfo() { return m_bookInfo; }
    TextFormatter *textFormatter() { return m_textFormat; }
    IndexDB *indexDB();

public:
    /**
      Open page with the given index

      This method should be used to handle index widget calls(ex. when double click on a title)
      @param pid page id, if pid == -1 then open the first page, if pid == -2 the open last page
      */
    virtual void openIndexID(int pid = -1);

    /**
      Open page at the given page and part number
      */
    virtual void goToPage(int page, int part);

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

      In quarn book, unit mean an AYA, so when calling this function on QuranDBHandler while
      highlight the next aya if it exists in the currentpage, or it go to the next page and
      highlight it.

      In a simple book unit mean visible part of page, calling this function on SimpleDBHandler
      will scroll the page down, and if it reach the end of page, calling it will open the next
      page.
      */
    virtual void nextUnit();

    /**
      Go to Previous unit
      */
    virtual void prevUnit();

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
      Check if this handler can have some books that need some time to load thier index model

      A fast index load mean that we should call first \ref topIndexModel and use
      threading to get the full index by calling \ref indexModel.
      @return True if it can take some time to load the full index model
      */
    bool needFastIndexLoad();
    /**
      Get the full index model of the curren book
      */
    virtual QAbstractItemModel *indexModel() = 0;
    /**
      Get only a top level index model
      */
    virtual QAbstractItemModel *topIndexModel()=0;
    virtual void getBookInfo() = 0;

protected:
    virtual void openPage(int page, int part=1) = 0;
    virtual void openID(int id = -1) = 0;
    virtual void connected();

protected:
    LibraryInfo *m_connetionInfo;
    BookInfo *m_bookInfo;
    IndexDB *m_indexDB;
    BookIndexModel *m_indexModel;
    TextFormatter *m_textFormat;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    QString m_bookDBPath;
    QString m_connectionName;
    bool m_fastIndex;
};

#endif // ABSTRACTDBHANDLER_H
