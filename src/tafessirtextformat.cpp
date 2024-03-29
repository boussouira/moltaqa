#include "tafessirtextformat.h"
#include "stringutils.h"
#include "utils.h"

TafessirTextFormat::TafessirTextFormat()
{
    m_cssID = "simpleBook";
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
    m_htmlHelper.insertImage("qrc:/images/arrow-down-double.png", QString(),
                             "style='width:16px; height: 16px;'");
    m_htmlHelper.endDiv();

    m_htmlHelper.beginDiv(".quran_text");
}

void TafessirTextFormat::endQuran()
{
    m_htmlHelper.endDiv();
    m_htmlHelper.endDiv();
}
