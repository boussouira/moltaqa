#ifndef BOOKINFOHANDLER_H
#define BOOKINFOHANDLER_H

#include "bookinfo.h"
#include <qdebug.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>

class BookInfoHandler
{
public:
    BookInfoHandler();
    ~BookInfoHandler();
    void openDB();
    BookInfo *getBookInfo(int bookID);

protected:
    QSqlDatabase m_indexDB;
    QSqlQuery *m_query;
    QString m_booksPath;
};

#endif // BOOKINFOHANDLER_H
