#ifndef TAFESSIRTEXTFORMAT_H
#define TAFESSIRTEXTFORMAT_H

#include "textformatter.h"

class TafessirTextFormat : public TextFormatter
{
public:
    TafessirTextFormat();
    void insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber);

    void beginQuran(QString soraName, int firstAya, int lastAya);
    void endQuran();

};

#endif // TAFESSIRTEXTFORMAT_H
