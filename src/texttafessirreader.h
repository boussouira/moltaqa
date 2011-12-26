#ifndef TEXTTAFESSIRREADER_H
#define TEXTTAFESSIRREADER_H

#include "textbookreader.h"

class TextTafessirReader : public TextBookReader
{
    Q_OBJECT
public:
    TextTafessirReader(QObject *parent = 0);
    ~TextTafessirReader();

    void firstPage();
    void setCurrentPage(QDomElement pageNode);

    QString text();
};

#endif // TEXTTAFESSIRREADER_H
