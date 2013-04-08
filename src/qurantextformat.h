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

    static QByteArray getAyaNumberImage(int aya, QString bgImage=QString());

    void setDrawAyaNumber(bool drawAyaNumber);
    void setUseDataProtocol(bool useDataProtocol);

protected:
    QString getAyaNumberImageData(int aya);

protected:
    QDir m_styleDir;
    bool m_drawAyaNumber;
    bool m_useDataProtocol;
};

#endif // QURANTEXTFORMAT_H
