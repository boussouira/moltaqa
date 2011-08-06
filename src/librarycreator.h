#ifndef LIBRARYCREATOR_H
#define LIBRARYCREATOR_H

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include "libraryinfo.h"
#include "shamelamanager.h"

class LibraryCreator
{
public:
    LibraryCreator();
    void openDB();
    void createTables();

    void start();
    void done();

    void addCat(CategorieInfo *cat);
    void addAuthor(AuthorInfo *auth);
    void addBook(ShamelaBookInfo *book);

protected:
    void importBook(ShamelaBookInfo *book, QString path);

public:
    LibraryInfo *m_library; //TODO: make this private

protected:
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    int m_prevArchive;
    QHash<int, int> m_catMap;
    QHash<int, int> m_levels;
};

#endif // LIBRARYCREATOR_H
