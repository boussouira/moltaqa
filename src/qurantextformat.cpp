#include "qurantextformat.h"

QuranTextFormat::QuranTextFormat()
{

}

void QuranTextFormat::insertSoraName(QString pSoraName)
{
    QString soraName = QObject::trUtf8("سورة %1").arg(pSoraName);
    m_text.append(QString("<div class=\"soraname\">%1</div>").arg(soraName));
}

void QuranTextFormat::insertBassemala()
{
    m_text.append(QString("<div class=\"txt\"><div class=\"bassemala\">%1</div>")
                  .arg(QObject::trUtf8("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ")));
}

void QuranTextFormat::insertAyaText(QString pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_text.append(QString("<span id=\"s%1a%2\" class=\"ayatxt\">%3</span>")
                  .arg(pSoraNumber).arg(pAyaNumber).arg(pAyaText));
    m_text.append(QString("<span class=\"ayanumber\">(%1)</span>").arg(pAyaNumber));
}

QString QuranTextFormat::text()
{
    return QString("<html>"
                   "<head>"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                   "<link href= \"qrc:/css/defaut.css\"  rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "<div id=\"quran\">%1</div>"
                   "</div></body></html>").arg(m_text);
}

void QuranTextFormat::clearQuranText()
{
    m_text.clear();
}

