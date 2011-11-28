#ifndef TEXTSIMPLEBOOKREADER_H
#define TEXTSIMPLEBOOKREADER_H

#include "textbookreader.h"

class TextSimpleBookReader : public TextBookReader
{
public:
    TextSimpleBookReader(QObject *parent=0);
    ~TextSimpleBookReader();

    void getTitles();
    void goFirst();

    void goToPage(int pid);
    void goToPage(int page, int part);

    bool hasPrev();
    bool hasNext();

    void nextPage();

    QString text();

protected:
    QHash<int, int> m_titles;
};

#endif // TEXTSIMPLEBOOKREADER_H
