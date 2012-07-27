#include "tafessirtextformat.h"
#include "utils.h"
#include "stringutils.h"

TafessirTextFormat::TafessirTextFormat()
{
    m_cssID = "simpleBook";
}

void TafessirTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    m_htmlHelper.insertDiv(soraName, "soraname");
}

void TafessirTextFormat::insertBassemala()
{
    m_htmlHelper.insertDiv(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void TafessirTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_htmlHelper.beginSpan(".aya");
    m_htmlHelper.insertSpan(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    m_htmlHelper.insertSpan(_u(" ﴿%1﴾ ").arg(Utils::String::Arabic::arabicNumbers(pAyaNumber)), ".ayanumber");
    m_htmlHelper.endSpan();
}

void TafessirTextFormat::beginQuran(QString soraName, int firstAya, int lastAya)
{
    QString quranTextInfo;
    if(firstAya < lastAya) {
        quranTextInfo = tr("سورة %1، من الاية %2 الى الاية %3")
                .arg(soraName)
                .arg(Utils::String::Arabic::arabicNumbers(firstAya))
                .arg(Utils::String::Arabic::arabicNumbers(lastAya));
    } else {
        quranTextInfo = tr("سورة %1، الاية %2")
                .arg(soraName)
                .arg(Utils::String::Arabic::arabicNumbers(lastAya));
    }

    m_htmlHelper.beginDiv(".tafessir_quran");

    m_htmlHelper.beginDiv(".toggale_quran");
    m_htmlHelper.insertSpan(quranTextInfo, ".quran_info");
    m_htmlHelper.insertImage("qrc:/images/add.png");
    m_htmlHelper.endDiv();

    m_htmlHelper.beginDiv(".quran_text");
}

void TafessirTextFormat::endQuran()
{
    m_htmlHelper.endDiv();
    m_htmlHelper.endDiv();
}
