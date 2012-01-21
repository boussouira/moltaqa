#include "qurantextformat.h"

QuranTextFormat::QuranTextFormat()
{
    m_cssID = "quran";
}


void QuranTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    m_htmlHelper.insertDivTag(soraName, ".soraname");
}

void QuranTextFormat::insertBassemala()
{
    m_htmlHelper.insertDivTag(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void QuranTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_htmlHelper.beginDivTag(".ayaDiv");
    m_htmlHelper.insertSpanTag(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    m_htmlHelper.insertSpanTag(QString("(%1)").arg(pAyaNumber), ".ayanumber");
    m_htmlHelper.endDivTag();
}
