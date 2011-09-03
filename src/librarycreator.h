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

#ifdef USE_MDBTOOLS
#include "mdbconvertermanager.h"
#endif

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
    void addTafessir(ShamelaBookInfo *tafessir);
    void addBook(ShamelaBookInfo *book);
    bool getShorooh(ShamelaBookInfo *mateen);
    bool getMateen(ShamelaBookInfo *shreeh);
    void addQuran();

    QList<ShamelaShareehInfo *> getShorooh();
    void addShareh(int mateenID, int mateenPage, int shareehID, int shareehPage);

protected:
    void importBook(ShamelaBookInfo *book, QString path);
    void importQuran(QString path);

    void readSimpleBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno);
    void readTafessirBook(ShamelaBookInfo *book, QSqlQuery &query, NewBookWriter &writer, bool hno);

    void getShorooh(int mateenID, int shareehID);

protected:
    QSqlDatabase m_bookDB;
    QSqlQuery m_bookQuery;
    LibraryInfo *m_library;
    ShamelaManager *m_shamelaManager;
    ShamelaInfo *m_shamelaInfo;
    ShamelaMapper *m_mapper;
    int m_prevArchive;
    int m_threadID;
    QHash<int, int> m_catMap;
    QHash<int, int> m_levels;
    bool m_importAuthor; ///< If true, import author's information when adding a book of him
    QMutex m_mutex;
    QList<ShamelaShareehInfo *> m_shorooh;

#ifdef USE_MDBTOOLS
    MdbConverterManager *m_mdbManager;
    QString m_tempDB;
#endif
};

#endif // LIBRARYCREATOR_H
