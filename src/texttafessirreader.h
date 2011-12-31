#ifndef TEXTTAFESSIRREADER_H
#define TEXTTAFESSIRREADER_H

#include "textbookreader.h"

class TextTafessirReader : public TextBookReader
{
    Q_OBJECT
public:
    TextTafessirReader(QObject *parent = 0);

    void setCurrentPage(QDomElement pageNode);
};

#endif // TEXTTAFESSIRREADER_H
