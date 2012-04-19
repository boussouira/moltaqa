#include "qurantextformat.h"

QuranTextFormat::QuranTextFormat()
{
    m_cssID = "quran";
}


void QuranTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    m_htmlHelper.insertDiv(soraName, ".soraname");
}

void QuranTextFormat::insertBassemala()
{
    m_htmlHelper.insertDiv(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void QuranTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_htmlHelper.beginDiv(".ayaDiv");
    m_htmlHelper.insertSpan(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    m_htmlHelper.insertSpan(QString("(%1)").arg(pAyaNumber), ".ayanumber");
    m_htmlHelper.endDiv();
}
