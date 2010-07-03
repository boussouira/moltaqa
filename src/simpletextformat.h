#ifndef SIMPLETEXTFORMAT_H
#define SIMPLETEXTFORMAT_H

#include <QString>
#include <QRegExp>

class SimpleTextFormat
{
public:
    SimpleTextFormat();
    void setText(const QString &txt) { m_cleanText = txt; }
    QString formatText();

protected:
    QString m_cleanText;
};

#endif // SIMPLETEXTFORMAT_H
