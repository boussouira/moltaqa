#ifndef ABSTRACTBOOKREADER_H
#define ABSTRACTBOOKREADER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qcoreapplication.h>
#include "librarymanager.h"
#include "sqlutils.h"

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
    void openBook(bool fastOpen=false);
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
    virtual void goToPage(int pid = -1)=0;

    /**
      Open page at the given page and part number
      */
    virtual void goToPage(int page, int part)=0;

    /**
      Open first page with given sora and aya number
      */
    virtual void goToSora(int sora, int aya);

    /**
      Open page with the given haddit number
      */
    virtual void goToHaddit(int hadditNum);

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

    static BookPage *getBookPage(LibraryBook *book, int pageID);

protected:
    /**
      Generate book info
      */
    virtual void getBookInfo();

    virtual void connected();

private:
    static BookPage *getSimpleBookPage(LibraryBook *book, int pageID);
    static BookPage *getTafessirPage(LibraryBook *book, int pageID);
    static BookPage *getQuranPage(LibraryBook *book, int pageID);

protected:
    Utils::DatabaseRemover m_remover;
    LibraryBook *m_bookInfo;
    BookPage *m_currentPage;
    LibraryManager *m_libraryManager;
    BookIndexModel *m_indexModel;
    QSqlDatabase m_bookDB;
    QString m_bookDBPath;
    QString m_connectionName;
};

#endif // ABSTRACTBOOKREADER_H
