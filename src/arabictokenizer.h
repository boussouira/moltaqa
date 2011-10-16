#ifndef ARABICTOKENIZER_H
#define ARABICTOKENIZER_H

#include "clheader.h"

class ArabicTokenizer: public CharTokenizer {
public:
    ArabicTokenizer(Reader* in);
    virtual ~ArabicTokenizer();

protected:
    bool isTokenChar(const TCHAR c) const;
    TCHAR normalize(const TCHAR chr) const;
};

#endif // ARABICTOKENIZER_H
