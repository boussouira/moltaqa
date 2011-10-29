#ifndef CSSFORMATTER_H
#define CSSFORMATTER_H

#include "clheader.h"
#include <qhash.h>

class CssFormatter : public Formatter {
public:

    CssFormatter();
    ~CssFormatter();

    TCHAR* highlightTerm(const TCHAR* originalText, const TokenGroup* group);

protected:
    int numHighlights;
    QList<QString> m_colors;
    QHash<QString, QString> m_wordColor;
    int m_colorIndex;
    bool m_useColor;
};


#endif // CSSFORMATTER_H
