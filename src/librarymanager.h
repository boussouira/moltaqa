#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "databasemanager.h"
#include "librarybook.h"

#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qhash.h>
#include <qstandarditemmodel.h>
#include <qtconcurrentrun.h>

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
class StatisticsManager;
class QuranAudioManager;

class LibraryManager : public DatabaseManager
{
    Q_OBJECT
public:
    LibraryManager(LibraryInfo *info, QObject *parent=0);
    ~LibraryManager();

    static LibraryManager *instance();

    LibraryInfo *libraryInfo();

    void loadModels();
    void clear();

    void openManagers();
    void reloadManagers();

    int addBook(ImportModelNode *node);
    void addBook(LibraryBook::Ptr book, int catID);

    void removeBook(int bookID);

    QHash<int, QAction *> textRefersActions();
    void addTextRefers(const QString &name, const QString &referText);
    void editRefers(int rid, const QString &name, const QString &referText);
    void deleteRefer(int rid);

    QHash<QString, QVariant> libraryStatistic();

    TaffesirListManager *taffesirListManager();
    BookListManager *bookListManager();
    LibraryBookManager *bookManager();
    AuthorsManager *authorsManager();
    TarajemRowatManager *rowatManager();
    FavouritesManager *favouritesManager();
    SearchManager *searchManager();
    QuranAudioManager *quranAudioManager();

    static int helpBookID() { return 101; }
    static QString helpBookUUID() { return "3491b579-a5e6-4eb7-b5b3-08a66f361849"; }

    void addHelpBook();

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
    StatisticsManager *m_statisticsManager;
    QuranAudioManager *m_quranAudioManager;
};

#endif // LIBRARYMANAGER_H
