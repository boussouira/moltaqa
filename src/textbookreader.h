#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"
#include "simplequery.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    void goFirst();
    void goToPage(int pid);
    void goToPage(int page, int part);

    bool hasPrev();
    bool hasNext();

    void nextPage();

    QString text();

protected:
    void connected();

protected:
    QString m_text;
};

#endif // TEXTBOOKREADER_H
