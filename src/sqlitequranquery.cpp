#include "sqlitequranquery.h"
#include "bookinfo.h"

#include <qvariant.h>

SqliteQuranQuery::SqliteQuranQuery()
{
}

SqliteQuranQuery::SqliteQuranQuery(QSqlDatabase db):
        QuranQuery(db)
{
}

SqliteQuranQuery::SqliteQuranQuery(QSqlDatabase db, BookInfo *bookInfo):
        QuranQuery(db, bookInfo)
{
}

void SqliteQuranQuery::page(int page)
{
    exec(QString("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                 "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                 "FROM QuranText LEFT JOIN QuranSowar "
                 "ON QuranSowar.id = QuranText.soraNumber "
                 "WHERE QuranText.pageNumber = %1 "
                 "ORDER BY QuranText.id").arg(page));
}

void SqliteQuranQuery::index()
{
    exec("SELECT id, soraName FROM QuranSowar ORDER BY id");
}

void SqliteQuranQuery::pageNumber(int aya, int sora)
{
    exec(QString("SELECT pageNumber "
                 "FROM QuranText "
                 "WHERE soraNumber = %1 AND ayaNumber = %2").arg(sora).arg(aya));
}

void SqliteQuranQuery::soraAyatCount(int sora)
{
    exec(QString("SELECT MAX(ayaNumber) "
                 "FROM QuranText "
                 "WHERE soraNumber = %1").arg(sora));
}

void SqliteQuranQuery::firstSoraAndAya(int page)
{
    exec(QString("SELECT MIN(soraNumber), MIN(ayaNumber) "
                 "FROM QuranText WHERE pageNumber = %1").arg(page));
}
