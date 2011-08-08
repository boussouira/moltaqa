#ifndef LIBRARYCREATOR_H
#define LIBRARYCREATOR_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qcoreapplication.h>
#include "libraryinfo.h"
#include "shamelamanager.h"
#include <qmutex.h>

class LibraryCreator
{
    Q_DECLARE_TR_FUNCTIONS(LibraryCreator)

public:
    LibraryCreator();
    void setImportAuthors(bool import);
    void setThreadID(int tid) { m_threadID = tid; }
    void openDB();
    void createTables();

    void start();
    void done();

    void importCats();
    void importAuthors();

    void addCat(CategorieInfo *cat);
    void addAuthor(AuthorInfo *auth, bool checkExist=false);
    void addBook(ShamelaBookInfo *book);

protected:
    void importBook(ShamelaBookInfo *book, QString path);

protected:
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    LibraryInfo *m_library;
    ShamelaManager *m_shamelaManager;
    ShamelaInfo *m_shamelaInfo;
    int m_prevArchive;
    int m_threadID;
    QHash<int, int> m_catMap;
    QHash<int, int> m_levels;
    bool m_importAuthor; ///< If true, import author's information when adding a book of him
    QMutex m_mutex;
};

#endif // LIBRARYCREATOR_H
