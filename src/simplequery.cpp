#include "simplequery.h"
#include "librarybook.h"
#include "utils.h"
#include <qvariant.h>
#include <qsqlerror.h>

SimpleQuery::SimpleQuery(QSqlDatabase db, LibraryBook *bookInfo) : QSqlQuery(db), m_bookInfo(bookInfo)
{
}

void SimpleQuery::nextPage(int id)
{
    exec(QString("SELECT id, pageText, partNum, pageNum from %1 "
                 "WHERE id >= %2 ORDER BY id ASC LIMIT 1")
         .arg(m_bookInfo->textTable)
         .arg(id));
}

void SimpleQuery::prevPage(int id)
{
    exec(QString("SELECT id, pageText, partNum, pageNum from %1 "
                 "WHERE id <= %2 ORDER BY id DESC LIMIT 1")
         .arg(m_bookInfo->textTable)
         .arg(id));
}

void SimpleQuery::page(int page, int part)
{
    exec(QString("SELECT id FROM %1 "
                 "WHERE pageNum >= %2 AND partNum = %3 "
                 "ORDER BY id ASC LIMIT 1")
         .arg(m_bookInfo->textTable)
         .arg(page)
         .arg(part));
}

void SimpleQuery::index()
{
    exec(QString("SELECT id, title, parentID FROM %1 ORDER BY id")
         .arg(m_bookInfo->indexTable));
}

int SimpleQuery::getHaddithNumber(int pageID)
{
    prepare(QString("SELECT haddit_number FROM bookMeta WHERE page_id = ?"));
    bindValue(0, pageID);

    if(exec()) {
        if(next()) {
            return value(0).toInt();
        }
    }

    return 0;
}

int SimpleQuery::getHaddithPage(int hadditNum)
{
    prepare(QString("SELECT page_id FROM bookMeta WHERE haddit_number = ?"));
    bindValue(0, hadditNum);

    if(exec()) {
        if(next()) {
            return value(0).toInt();
        }
    }

    return 0;
}
