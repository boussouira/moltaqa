#ifndef TAFESSIRTEXTFORMAT_H
#define TAFESSIRTEXTFORMAT_H

#include "qurantextformat.h"

class TafessirTextFormat : public QuranTextFormat
{
public:
    TafessirTextFormat();

    void beginQuran(QString soraName, int firstAya, int lastAya);
    void endQuran();
};

#endif // TAFESSIRTEXTFORMAT_H
