#include "bookinfohandler.h"

BookInfoHandler::BookInfoHandler()
{
    QSettings settings;
    m_appDir = settings.value("General/app_dir").toString();

    openDB();
}

BookInfoHandler::~BookInfoHandler()
{
    delete m_query;
    m_indexDB.close();
}

void BookInfoHandler::openDB()
{
    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "BooksInfoDB");
    m_indexDB.setDatabaseName(QString("%1/books/books_index.db").arg(m_appDir));
    if(!m_indexDB.open())
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    m_query = new QSqlQuery(m_indexDB);
}

BookInfo *BookInfoHandler::getBookInfo(int bookID)
{
    BookInfo *bookInfo = new BookInfo();
    m_query->exec(QString("SELECT bookName, bookType, fileName "
                          "From booksList where id = %1 LIMIT 1").arg(bookID));
    if(m_query->next()) {
        bookInfo->setBookName(m_query->value(0).toString());
        bookInfo->setBookType((BookInfo::Type)m_query->value(1).toInt());
        bookInfo->setBookID(bookID);
        bookInfo->setBookPath(QString("%1/books/%2")
                              .arg(m_appDir)
                              .arg(m_query->value(2).toString()));
    } else
        qDebug() << "SQL error:" << m_query->lastError().text();

    return bookInfo;
}
