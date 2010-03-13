#include "tafessirtextformat.h"
#include <qdebug.h>

TafessirTextFormat::TafessirTextFormat()
{
}
void TafessirTextFormat::appendTafessirText(QString pTafessirText)
{
    //qDebug() << pTafessirText.split(QRegExp("\\[co\\]")).count() ;
    pTafessirText.replace(QRegExp("\\(([0-9]{1,2})\\)"), "<a href=\"#fn\\1\">(\\1)</a>");
    pTafessirText.replace("[br]","<br/>");
    pTafessirText.replace("[co]","<br>______<br><code>");
    pTafessirText.replace("[/co]","</code>");
    m_text.append(pTafessirText);
}

QString TafessirTextFormat::getTafessirText()
{
    return QString("<html><heade><style>body{border:1px solid #CCCCCC;direction:rtl;font-family:Traditional arabic;font-size:30px;padding:5px;text-align:right;}"
                   "code{font-size:20px}</style>"
                   "</head><body>%1</body></html>").arg(m_text);
}

void TafessirTextFormat::clearTafessirText()
{
    m_text.clear();
}
