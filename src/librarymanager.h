#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qcoreapplication.h>
#include <qhash.h>
#include <QtConcurrentRun>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qstandarditemmodel.h>
#include <QTime>
#include "libraryenums.h"

class QDomElement;
class LibraryInfo;
class LibraryBook;
class BookPage;
class ImportModelNode;
class LibraryBookManager;
class TaffesirListManager;
class BookListManager;

class LibraryManager : public QObject
{
    Q_OBJECT
public:
    LibraryManager(LibraryInfo *info, QObject *parent=0);
    ~LibraryManager();

    /**
      Open the index database
      @throw BookException
      */
    void open();
    void openManagers();

    LibraryInfo *connectionInfo();
    QPair<int, QString> findAuthor(const QString &name);
    int addBook(ImportModelNode *node);
    void addBook(LibraryBook *book, int catID);

    bool hasShareeh(int bookID);
    void getShoroohPages(LibraryBook *info, BookPage *page);

    QStandardItemModel *getAuthorsListModel();
    void updateBookInfo(LibraryBook *info);

    void setBookIndexStat(int bookID, Enums::indexFlags indexFlag);
    QList<int> getNonIndexedBooks();

    TaffesirListManager *taffesirListManager();
    BookListManager *bookListManager();
    LibraryBookManager *bookManager();

signals:
    void bookAdded();

protected:
    LibraryInfo *m_libraryInfo;
    QSqlDatabase m_indexDB;
    QString m_connName;
    QHash<int, LibraryBook*> m_savedBook;
    LibraryBookManager *m_bookmanager;
    TaffesirListManager *m_taffesirManager;
    BookListManager *m_bookListManager;
};

#endif // LIBRARYMANAGER_H
