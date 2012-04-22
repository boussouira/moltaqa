#ifndef LIBRARYCREATOR_H
#define LIBRARYCREATOR_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qcoreapplication.h>
#include <qmutex.h>

#include "libraryinfo.h"
#include "shamelamanager.h"
#include "newbookwriter.h"
#include "shamelamapper.h"
#include "librarymanager.h"
#include "sqlutils.h"

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

    void readSimpleBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno);
    void readTafessirBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno);

protected:
    DatabaseRemover m_remover;
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
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
