#ifndef QURANTEXTFORMAT_H
#define QURANTEXTFORMAT_H

#include "textformatter.h"

class QuranTextFormat : public TextFormatter
{
public:
    QuranTextFormat();
    void insertSoraName(QString pSoraName);
    void insertBassemala();
    void insertAyaText(QString pAyaText, int pAyaNumber, int pSoraNumber);
    QString text();
    void clearQuranText();

private:
    QString m_text;

};

#endif // QURANTEXTFORMAT_H
