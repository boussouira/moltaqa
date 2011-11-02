#ifndef TEXTQURANREADER_H
#define TEXTQURANREADER_H

#include "textbookreader.h"

class TextQuranReader : public TextBookReader
{
public:
    TextQuranReader(QObject *parent=0);

    void goFirst();

    void goToPage(int pid);
    void goToPage(int page, int part);

    void nextPage();

    bool hasNext();
    bool hasPrev();

    QString text();
};

#endif // TEXTQURANREADER_H