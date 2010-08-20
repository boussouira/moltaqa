#include "qurantextformat.h"

QuranTextFormat::QuranTextFormat()
{
    m_cssID = "quran";
}

void QuranTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = QObject::trUtf8("سورة %1").arg(pSoraName);
    insertDivTag(soraName, "soraname");
}

void QuranTextFormat::insertBassemala()
{
    insertDivTag(QObject::trUtf8("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), "bassemala");
}

void QuranTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    insertSpanTag(pAyaText, "ayatxt", QString("s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    insertSpanTag(QString("(%1)").arg(pAyaNumber), "ayanumber");
}
