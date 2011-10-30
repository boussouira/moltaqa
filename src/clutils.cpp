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

wchar_t* Utils::intToWChar(int num, wchar_t *dest, int radix)
{
    return _itow(num, dest, radix);
}

wchar_t* Utils::intToWChar(int num, int radix)
{
    wchar_t *str = (wchar_t*) malloc(128 * sizeof(wchar_t));
    return Utils::intToWChar(num, str, radix);
}

int Utils::WCharToInt(const wchar_t *str)
{
    return _wtoi(str);
}

QString Utils::highlightText(QString orignalText, lucene::search::Query *query, bool fragment)
{
    // Highlight the text
    ArabicAnalyzer hl_analyzer;
    SpanHighlightScorer scorer( true );
    CssFormatter hl_formatter;

    int maxNumFragments = fragment ? 5 : 100; // Fragment count
    int fragmentSize = fragment ? 100 : 10000; // Fragment lenght
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
