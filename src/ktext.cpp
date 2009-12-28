#include "ktext.h"

KText::KText()
{

}

void KText::insertSoraName(QString pSoraName)
{
    m_text.append(QString("<div class=\"soraname\">%1 %2</div>").arg(SORAT).arg(pSoraName));
}

void KText::insertBassemala()
{
    m_text.append(QString("<div class=\"txt\"><div class=\"bassemala\">%1</div>").arg(BASMALA));
}

void KText::insertAyaText(QString pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_text.append(QString("<span id=\"s%1a%2\" class=\"ayatxt\">%3</span>")
                  .arg(pSoraNumber).arg(pAyaNumber).arg(pAyaText));
    m_text.append(QString("<span class=\"ayanumber\">(%1)</span>").arg(pAyaNumber));
}

QString KText::text()
{
    return QString("<html>"
                   "<head>"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                   "<link href= \"qrc:/css/defaut.css\"  rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "%1"
                   "</div></body></html>").arg(m_text);
}

void KText::clear()
{
    m_text.clear();
}

