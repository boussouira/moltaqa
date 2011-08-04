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
    exec(QString("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                 "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                 "FROM QuranText LEFT JOIN QuranSowar "
                 "ON QuranSowar.id = QuranText.soraNumber "
                 "WHERE QuranText.pageNumber = %1 "
                 "ORDER BY QuranText.id").arg(page));
}

void QuranQuery::index()
{
    exec("SELECT id, soraName FROM QuranSowar ORDER BY id");
}

void QuranQuery::pageNumber(int aya, int sora)
{
    exec(QString("SELECT pageNumber "
                 "FROM QuranText "
                 "WHERE soraNumber = %1 AND ayaNumber = %2").arg(sora).arg(aya));
}

void QuranQuery::soraAyatCount(int sora)
{
    exec(QString("SELECT MAX(ayaNumber) "
                 "FROM QuranText "
                 "WHERE soraNumber = %1").arg(sora));
}

void QuranQuery::firstSoraAndAya(int page)
{
    exec(QString("SELECT MIN(soraNumber), MIN(ayaNumber) "
                 "FROM QuranText WHERE pageNumber = %1").arg(page));
}
