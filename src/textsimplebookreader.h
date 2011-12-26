#ifndef TEXTSIMPLEBOOKREADER_H
#define TEXTSIMPLEBOOKREADER_H

#include "textbookreader.h"

class TextSimpleBookReader : public TextBookReader
{
public:
    TextSimpleBookReader(QObject *parent=0);
    ~TextSimpleBookReader();

    void firstPage();
    void setCurrentPage(QDomElement pageNode);

    QString text();
};

#endif // TEXTSIMPLEBOOKREADER_H
