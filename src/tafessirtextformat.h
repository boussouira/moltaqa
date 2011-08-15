#ifndef TAFESSIRTEXTFORMAT_H
#define TAFESSIRTEXTFORMAT_H

#include "textformatter.h"

class TafessirTextFormat : public TextFormatter
{
public:
    TafessirTextFormat();
    void insertSoraName(const QString &pSoraName);
    void insertBassemala();
    void insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber);

    void beginQuran();
    void endQuran();

};

#endif // TAFESSIRTEXTFORMAT_H
