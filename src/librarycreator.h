#ifndef LIBRARYCREATOR_H
#define LIBRARYCREATOR_H

#include <qcoreapplication.h>
#include <qhash.h>
#include <qmutex.h>

class LibraryInfo;
class ShamelaManager;
class ShamelaMapper;
class LibraryManager;
class AuthorsManager;
class BookListManager;
class TaffesirListManager;
class ShamelaInfo;
class ShamelaCategorieInfo;
class ShamelaAuthorInfo;
class ShamelaBookInfo;

class LibraryCreator
{
    Q_DECLARE_TR_FUNCTIONS(LibraryCreator)

public:
    LibraryCreator();
    ~LibraryCreator();
    void setThreadID(int tid) { m_threadID = tid; }
    void openDB();

    void start();
    void done();

    void importCats();
    void importAuthors();

    void addCat(ShamelaCategorieInfo *cat);
    void addAuthor(ShamelaAuthorInfo *auth, bool checkExist=false);
    void addTafessir(ShamelaBookInfo *tafessir);
    void addBook(ShamelaBookInfo *book);
    void addQuran();

protected:
    void importBook(ShamelaBookInfo *shamelBook, QString path);
    void importQuran(QString path);

protected:
    LibraryInfo *m_library;
    ShamelaManager *m_shamelaManager;
    ShamelaInfo *m_shamelaInfo;
    ShamelaMapper *m_mapper;
    LibraryManager *m_libraryManager;
    AuthorsManager *m_authorsManager;
    BookListManager *m_bookListManager;
    TaffesirListManager *m_taffesirManager;
    int m_prevArchive;
    int m_threadID;
    QHash<int, int> m_levels;
    bool m_importAuthor; ///< If true, import author's information when adding a book of him
    QMutex m_mutex;
    QString m_tempDB; // Need this if USE_MDBTOOLS is defined
};

#endif // LIBRARYCREATOR_H
