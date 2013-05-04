#ifndef ARABICFILTER_H
#define ARABICFILTER_H

#include "clheader.h"

class ArabicFilter: public TokenFilter {
public:
    ArabicFilter(TokenStream* in, bool deleteTs);
    virtual ~ArabicFilter();

    Token* next(Token* token);
};

#endif // ARABICFILTER_H
