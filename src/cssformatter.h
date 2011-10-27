#ifndef CSSFORMATTER_H
#define CSSFORMATTER_H

#include "clheader.h"

class CssFormatter : public Formatter {
public:

    CssFormatter();
    ~CssFormatter();

    TCHAR* highlightTerm(const TCHAR* originalText, const TokenGroup* group);

protected:
    int numHighlights;
};


#endif // CSSFORMATTER_H
