#ifndef TAFESSIRTEXTFORMAT_H
#define TAFESSIRTEXTFORMAT_H

#include <QtCore>

class TafessirTextFormat
{
public:
    TafessirTextFormat();

protected:
    void appendTafessirText(QString pTafessirText);
    QString getTafessirText();
    void clearTafessirText();

private:
    QString m_text;
};

#endif // TAFESSIRTEXTFORMAT_H
