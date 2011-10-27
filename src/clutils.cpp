#include "clutils.h"
#include "clconstants.h"
#include "clheader.h"
#include "arabicanalyzer.h"
#include "cssformatter.h"
#include <stdlib.h>
#include <qregexp.h>

QString Utils::WCharToString(const wchar_t *str)
{
    return QString::fromWCharArray(str);
}

wchar_t* Utils::QStringToWChar(const QString &str)
{
    wchar_t *string = (wchar_t*) malloc((str.length()+1) * sizeof(wchar_t));
    str.toWCharArray(string);
    string[str.length()] = 0;

    return string;
}

wchar_t* Utils::intToWChar(int num, int radix)
{
    wchar_t *str = (wchar_t*) malloc(10 * sizeof(wchar_t));
    _itow(num, str, radix);

    return str;
}

QString Utils::highlightText(QString orignalText, lucene::search::Query *query)
{
    // Highlight the text
    ArabicAnalyzer hl_analyzer;
    SpanHighlightScorer scorer( true );
    CssFormatter hl_formatter;
    int maxNumFragments = 5; // Fragment count
    int fragmentSize = 100; // Fragment lenght
    const wchar_t* fragmentSeparator = _T("...");

    Highlighter highlighter(&hl_formatter, &scorer);
    SimpleFragmenter frag(fragmentSize);
    highlighter.setTextFragmenter(&frag);

    orignalText.replace(QRegExp("[\\r\\n]"),"<br/>");

    wchar_t* text = QStringToWChar(orignalText);
    StringReader reader(text);
    CachingTokenFilter tokenStream( hl_analyzer.tokenStream( PAGE_TEXT_FIELD, &reader ), true);
    scorer.init( query, PAGE_TEXT_FIELD, &tokenStream );
    tokenStream.reset();

    wchar_t* hi_result = highlighter.getBestFragments(
            &tokenStream,
            text,
            maxNumFragments,
            fragmentSeparator);

    QString highlightedText = WCharToString(hi_result);

    _CLDELETE_CARRAY(hi_result)
    _CLDELETE_CARRAY(text);

    return highlightedText;
}
