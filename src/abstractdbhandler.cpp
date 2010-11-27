#include "abstractdbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"
#include "bookexception.h"

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qstringlistmodel.h>

AbstractDBHandler::AbstractDBHandler()
{
    m_indexModel = new BookIndexModel();
    m_bookInfo = new BookInfo();
}

AbstractDBHandler::~AbstractDBHandler()
{
    delete m_indexModel;
    delete m_bookInfo;
}

void AbstractDBHandler::openBookDB(QString pBookDBPath)
{
    QString bookPath = pBookDBPath.isEmpty() ? m_bookInfo->bookPath() : pBookDBPath;
    if(QSqlDatabase::contains(m_connectionName)) {
        m_bookDB = QSqlDatabase::database(m_connectionName);
    } else {
        m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        m_bookDB.setDatabaseName(bookPath);
    }

    if (!m_bookDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات"), bookPath);

    m_bookQuery =  QSqlQuery(m_bookDB);
    getBookInfo();
}

void AbstractDBHandler::setBookInfo(BookInfo *bi)
{
    m_bookInfo = bi;
    m_connectionName = QString("book_i%1").arg(m_bookInfo->bookID());
}

QString AbstractDBHandler::nextUnit()
{
    return QString();
}

QString AbstractDBHandler::prevUnit()
{
    return QString();
}

QString AbstractDBHandler::openIndexID(int pid)
{
    return openID(pid);
}
