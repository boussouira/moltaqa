#ifndef TEXTSIMPLEBOOKREADER_H
#define TEXTSIMPLEBOOKREADER_H

#include "textbookreader.h"

class TextSimpleBookReader : public TextBookReader
{
public:
    TextSimpleBookReader(QObject *parent=0);

    void setCurrentPage(QDomElement pageNode);

protected:
    int m_lastTitleID;
};

#endif // TEXTSIMPLEBOOKREADER_H
