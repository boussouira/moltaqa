#include "qurantextformat.h"

QuranTextFormat::QuranTextFormat()
{
    m_cssID = "quran";
}


void QuranTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    insertDivTag(soraName, ".soraname");
}

void QuranTextFormat::insertBassemala()
{
    insertDivTag(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void QuranTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    openHtmlTag("div", ".ayaDiv");
    insertSpanTag(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    insertSpanTag(QString("(%1)").arg(pAyaNumber), ".ayanumber");
    closeHtmlTag();
}
