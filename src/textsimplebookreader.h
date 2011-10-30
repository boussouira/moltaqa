#ifndef TEXTSIMPLEBOOKREADER_H
#define TEXTSIMPLEBOOKREADER_H

#include "textbookreader.h"

class TextSimpleBookReader : public TextBookReader
{
public:
    TextSimpleBookReader(QObject *parent=0);
    ~TextSimpleBookReader();

    void goFirst();

    void goToPage(int pid);
    void goToPage(int page, int part);

    bool hasPrev();
    bool hasNext();

    void nextPage();

    QString text();
};

#endif // TEXTSIMPLEBOOKREADER_H
