#include "tafessirtextformat.h"

TafessirTextFormat::TafessirTextFormat()
{
    m_cssID = "simpleBook";
}

void TafessirTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    insertDivTag(soraName, "soraname");
}

void TafessirTextFormat::insertBassemala()
{
    insertDivTag(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), "bassemala");
}

void TafessirTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    insertSpanTag(pAyaText, "ayatxt", QString("s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    insertSpanTag(QString("(%1)").arg(pAyaNumber), "ayanumber");
}

void TafessirTextFormat::beginQuran()
{
    m_text.append("<div class=\"tafessir_quran\">");
}

void TafessirTextFormat::endQuran()
{
    m_text.append("</div>");
}