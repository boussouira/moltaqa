#ifndef TEXTSIMPLEBOOKREADER_H
#define TEXTSIMPLEBOOKREADER_H

#include "textbookreader.h"

class TextSimpleBookReader : public TextBookReader
{
public:
    TextSimpleBookReader(QObject *parent=0);

    void setCurrentPage(QDomElement pageNode);
};

#endif // TEXTSIMPLEBOOKREADER_H
