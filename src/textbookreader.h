#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"
#include "simplequery.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    void goToPage(int pid);
    void goToPage(int page, int part);
    QString text();

protected:
    void connected();

protected:
    SimpleQuery *m_simpleQuery;
    QString m_text;
};

#endif // TEXTBOOKREADER_H
