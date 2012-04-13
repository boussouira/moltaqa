#include "clutils.h"
#include "clconstants.h"
#include "clheader.h"
#include "arabicanalyzer.h"
#include "cssformatter.h"
#include "clucenequery.h"
#include <stdlib.h>
#include <qregexp.h>

namespace Utils {
namespace CLucene {

wchar_t* QStringToWChar(const QString &str)
{
    wchar_t *string = (wchar_t*) malloc((str.length()+1) * sizeof(wchar_t));
    str.toWCharArray(string);
    string[str.length()] = 0;

    return string;
}

wchar_t* intToWChar(int num, int radix)
{
    wchar_t *str = (wchar_t*) malloc(128 * sizeof(wchar_t));
    return intToWChar(num, str, radix);
}

QString highlightText(QString orignalText, lucene::search::Query *query,
                      const wchar_t *field, bool fragment)
{
    // Highlight the text
    ArabicAnalyzer analyzer;
    SpanHighlightScorer scorer( true );
    CssFormatter formatter;

    int maxNumFragments = fragment ? 5 : 100; // Fragment count
    int fragmentSize = fragment ? 100 : 10000; // Fragment lenght
    const wchar_t* fragmentSeparator = _T("...");

    Highlighter highlighter(&formatter, &scorer);
    SimpleFragmenter frag(fragmentSize);
    highlighter.setTextFragmenter(&frag);

    orignalText.replace(QRegExp("[\\r\\n]"),"<br/>"); // TODO: delete this and make orignalText a const reference

    wchar_t* text = QStringToWChar(orignalText);
    StringReader reader(text);
    CachingTokenFilter tokenStream(analyzer.tokenStream(field, &reader), true);
    scorer.init(query, field, &tokenStream);
    tokenStream.reset();

    wchar_t* highlighterResult = highlighter.getBestFragments(
            &tokenStream,
            text,
            maxNumFragments,
            fragmentSeparator);

    QString highlightedText = WCharToString(highlighterResult);

    _CLDELETE_CARRAY(highlighterResult)
    _CLDELETE_CARRAY(text);

    return highlightedText;
}

QString highlightText(QString orignalText, CLuceneQuery *query, bool fragment)
{
    return highlightText(orignalText, query->searchQuery, query->searchFieldW, fragment);
}

}
}
