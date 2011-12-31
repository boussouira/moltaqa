#ifndef TEXTQURANREADER_H
#define TEXTQURANREADER_H

#include "textbookreader.h"

class TextQuranReader : public TextBookReader
{
public:
    TextQuranReader(QObject *parent=0);

    void setCurrentPage(QDomElement pageNode);

protected:
    void getTitles();
    void getPages();
};

#endif // TEXTQURANREADER_H
