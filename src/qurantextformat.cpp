#include "qurantextformat.h"
#include "utils.h"
#include "stringutils.h"

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
    m_htmlHelper.beginSpan(".aya");
    m_htmlHelper.insertSpan(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    m_htmlHelper.insertSpan(_u(" ﴿%1﴾ ").arg(Utils::String::Arabic::arabicNumbers(pAyaNumber)), ".ayanumber");
    m_htmlHelper.endSpan();
}
