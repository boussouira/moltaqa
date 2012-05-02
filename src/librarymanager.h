#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <qcoreapplication.h>
#include <qhash.h>
#include <qtconcurrentrun.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qstandarditemmodel.h>
#include <qdatetime.h>
#include "databasemanager.h"
#include "librarybook.h"

class QDomElement;
class LibraryInfo;
class LibraryBook;
class BookPage;
class ImportModelNode;
class ListManager;
class LibraryBookManager;
class TaffesirListManager;
class BookListManager;
class AuthorsManager;
class TarajemRowatManager;
class FavouritesManager;
class SearchManager;
class QAction;

class LibraryManager : public DatabaseManager
{
    Q_OBJECT
public:
    LibraryManager(LibraryInfo *info, QObject *parent=0);
    ~LibraryManager();

    static LibraryManager *instance();

    void loadModels();
    void clear();

    void openManagers();
    void reloadManagers();

    int addBook(ImportModelNode *node);
    void addBook(LibraryBookPtr book, int catID);

    void removeBook(int bookID);

    QHash<int, QAction *> textRefersActions();
    void addTextRefers(const QString &name, const QString &referText);
    void editRefers(int rid, const QString &name, const QString &referText);
    void deleteRefer(int rid);

    TaffesirListManager *taffesirListManager();
    BookListManager *bookListManager();
    LibraryBookManager *bookManager();
    AuthorsManager *authorsManager();
    TarajemRowatManager *rowatManager();
    FavouritesManager *favouritesManager();
    SearchManager *searchManager();

protected:
    LibraryInfo *m_libraryInfo;
    QList<ListManager*> m_managers;
    LibraryBookManager *m_bookmanager;
    TaffesirListManager *m_taffesirManager;
    BookListManager *m_bookListManager;
    AuthorsManager *m_authorsManager;
    TarajemRowatManager *m_rowatManager;
    FavouritesManager *m_favourites;
    SearchManager *m_searchManager;
};

#endif // LIBRARYMANAGER_H
