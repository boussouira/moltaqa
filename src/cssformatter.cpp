#include "cssformatter.h"

CssFormatter::CssFormatter() : numHighlights(0)
{
}

CssFormatter::~CssFormatter()
{
}

TCHAR * CssFormatter::highlightTerm(const TCHAR *originalText, const lucene::search::highlight::TokenGroup *group)
{
    if(group->getTotalScore()>0){
        StringBuffer sb;
        sb.append(_T("<b style=\"background-color:#ffff63\">"));
        sb.append(originalText);
        sb.append(_T("</b>"));

        numHighlights++;

        return sb.toString();
    }

    return stringDuplicate(originalText);
}
