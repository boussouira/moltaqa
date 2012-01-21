#include "tafessirtextformat.h"

TafessirTextFormat::TafessirTextFormat()
{
    m_cssID = "simpleBook";
}

void TafessirTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    m_htmlHelper.insertDivTag(soraName, "soraname");
}

void TafessirTextFormat::insertBassemala()
{
    m_htmlHelper.insertDivTag(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void TafessirTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_htmlHelper.beginDivTag(".ayaDiv");
    m_htmlHelper.insertSpanTag(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    m_htmlHelper.insertSpanTag(QString("(%1)").arg(pAyaNumber), ".ayanumber");
    m_htmlHelper.endDivTag();
}

void TafessirTextFormat::beginQuran(QString soraName, int firstAya, int lastAya)
{
    QString quranTextInfo;
    if(firstAya < lastAya) {
        quranTextInfo = tr("سورة %1، من الاية %2 الى الاية %3")
                .arg(soraName).arg(firstAya).arg(lastAya);
    } else {
        quranTextInfo = tr("سورة %1، الاية %2").arg(soraName).arg(firstAya);
    }

    m_htmlHelper.beginDivTag(".tafessir_quran");

    m_htmlHelper.beginDivTag(".toggale_quran");
    m_htmlHelper.insertSpanTag(quranTextInfo, ".quran_info");
    m_htmlHelper.insertImage("qrc:/menu/images/add.png");
    m_htmlHelper.endDivTag();

    m_htmlHelper.beginDivTag(".quran_text");
}

void TafessirTextFormat::endQuran()
{
    m_htmlHelper.endDivTag();
    m_htmlHelper.endDivTag();
}
