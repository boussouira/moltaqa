#ifndef SIMPLETEXTFORMAT_H
#define SIMPLETEXTFORMAT_H

#include "textformatter.h"

class SimpleTextFormat : public TextFormatter
{
public:
    SimpleTextFormat();
    void setText(const QString &txt) { m_cleanText = txt; }
    QString formatText();

protected:
    QString m_cleanText;
};

#endif // SIMPLETEXTFORMAT_H
