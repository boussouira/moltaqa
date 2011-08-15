#ifndef QURANTEXTFORMAT_H
#define QURANTEXTFORMAT_H

#include "textformatter.h"

class QuranTextFormat : public TextFormatter
{
public:
    QuranTextFormat();

    void insertSoraName(const QString &pSoraName);
    void insertBassemala();
    void insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber);
};

#endif // QURANTEXTFORMAT_H
