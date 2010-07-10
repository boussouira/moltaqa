#include "bookinfohandler.h"

BookInfoHandler::BookInfoHandler()
{
    QSettings settings;
    m_booksPath = settings.value("app/db").toString();
    if(m_booksPath.endsWith(QChar('/')) || m_booksPath.endsWith(QChar('\\')))
        m_booksPath.remove(m_booksPath.size()-1, 1);

    openDB();
}

BookInfoHandler::~BookInfoHandler()
{
    m_indexDB.close();
    delete m_query;
}

void BookInfoHandler::openDB()
{
    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "BooksInfoDB");
    m_indexDB.setDatabaseName(QString("%1/books_index.db").arg(m_booksPath));
    if(!m_indexDB.open())
        qDebug() << "Can not open database";
    m_query = new QSqlQuery(m_indexDB);
}

BookInfo *BookInfoHandler::getBookInfo(int bookID)
{
    BookInfo *bookInfo = new BookInfo();
    m_query->exec(QString("SELECT bookName, bookType, fileName "
                          "From booksList where id = %1 LIMIT 1").arg(bookID));
    if(m_query->first()) {
        bookInfo->setBookName(m_query->value(0).toString());
        bookInfo->setBookType((BookInfo::Type)m_query->value(1).toInt());
        bookInfo->setBookID(bookID);
        bookInfo->setBookPath(QString("%1/%2")
                              .arg(m_booksPath)
                              .arg(m_query->value(2).toString()));
    } else
        qDebug() << "SQL error:" << m_query->lastError().text();

    return bookInfo;
}
