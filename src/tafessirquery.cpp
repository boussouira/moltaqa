#include "tafessirquery.h"
#include "bookinfo.h"
#include "common.h"
#include <qsqlerror.h>
#include <qvariant.h>
#include <qdebug.h>

TafessirQuery::TafessirQuery(QSqlDatabase db, BookInfo *bookInfo) : QSqlQuery(db), m_bookInfo(bookInfo)
{
}

void TafessirQuery::nextPage(int id)
{
    prepare("SELECT bookPages.id, bookPages.pageText, bookPages.partNum, bookPages.pageNum, tafessirMeta.aya_number, tafessirMeta.sora_number "
            "FROM bookPages "
            "LEFT JOIN tafessirMeta "
            "ON tafessirMeta.page_id = bookPages.id "
            "WHERE bookPages.id >= ? "
            "ORDER BY bookPages.id ASC LIMIT 1 ");

    bindValue(0, id);
    exec();
}

void TafessirQuery::prevPage(int id)
{
    prepare("SELECT bookPages.id, bookPages.pageText, bookPages.partNum, bookPages.pageNum, tafessirMeta.aya_number, tafessirMeta.sora_number "
            "FROM bookPages "
            "LEFT JOIN tafessirMeta "
            "ON tafessirMeta.page_id = bookPages.id "
            "WHERE bookPages.id <= ? "
            "ORDER BY bookPages.id DESC LIMIT 1 ");

    bindValue(0, id);
    exec();
}


void TafessirQuery::page(int page, int part)
{
    exec(QString("SELECT id FROM bookPages "
                 "WHERE pageNum >= %1 AND partNum = %2 "
                 "ORDER BY id ASC LIMIT 1")
         .arg(page)
         .arg(part));
}

int TafessirQuery::getAyatCount(int sora, int aya)
{
    exec(QString("SELECT aya_number FROM tafessirMeta "
                 "WHERE sora_number = %1 AND aya_number > %2 "
                 "ORDER BY page_id ASC LIMIT 1")
         .arg(sora)
         .arg(aya));

    if(next()) {
        return value(0).toInt() - aya;
    }

    return 0;
}

int TafessirQuery::getPageID(int sora, int aya)
{
    exec(QString("SELECT page_id FROM tafessirMeta "
                 "WHERE sora_number = %1 AND aya_number <= %2 "
                 "ORDER BY page_id DESC LIMIT 1")
         .arg(sora)
         .arg(aya));

    if(next()) {
        return value(0).toInt();
    }

    return 0;
}

int TafessirQuery::getHaddithPage(int hadditNum)
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
