#include "quranquery.h"

QuranQuery::QuranQuery()
{
}

QuranQuery::QuranQuery(QSqlDatabase db) : QSqlQuery(db)
{
}

QuranQuery::QuranQuery(QSqlDatabase db, BookInfo *bookInfo) : QSqlQuery(db), m_bookInfo(bookInfo)
{
}

void QuranQuery::page(int page)
{
    exec(QString("SELECT quranText.id, quranText.ayaText, quranText.ayaNumber, "
                 "quranText.pageNumber, quranText.soraNumber, quranSowar.SoraName "
                 "FROM quranText LEFT JOIN quranSowar "
                 "ON quranSowar.id = quranText.soraNumber "
                 "WHERE quranText.pageNumber = %1 "
                 "ORDER BY quranText.id").arg(page));
}

void QuranQuery::index()
{
    exec("SELECT id, soraName FROM quranSowar ORDER BY id");
}

void QuranQuery::pageNumber(int aya, int sora)
{
    exec(QString("SELECT pageNumber "
                 "FROM quranText "
                 "WHERE soraNumber = %1 AND ayaNumber = %2").arg(sora).arg(aya));
}

void QuranQuery::soraAyatCount(int sora)
{
    exec(QString("SELECT MAX(ayaNumber) "
                 "FROM quranText "
                 "WHERE soraNumber = %1").arg(sora));
}

void QuranQuery::firstSoraAndAya(int page)
{
    exec(QString("SELECT soraNumber, ayaNumber "
                 "FROM quranText WHERE pageNumber = %1 "
                 "ORDER BY id LIMIT 1").arg(page));
}
