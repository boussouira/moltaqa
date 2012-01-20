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
    insertDivTag(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void TafessirTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    openHtmlTag("div", ".ayaDiv");
    insertSpanTag(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));
    insertSpanTag(QString("(%1)").arg(pAyaNumber), ".ayanumber");
    closeHtmlTag();
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

    openHtmlTag("div", ".tafessir_quran");

    openHtmlTag("div", ".toggale_quran");
    insertSpanTag(quranTextInfo, ".quran_info");
    insertImage("qrc:/menu/images/add.png");
    closeHtmlTag();

    openHtmlTag("div", ".quran_text");
}

void TafessirTextFormat::endQuran()
{
    closeHtmlTag();
    closeHtmlTag();
}
