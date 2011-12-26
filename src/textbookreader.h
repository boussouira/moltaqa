#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"
#include "simplequery.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    void firstPage();
    virtual void getTitles();

protected:
     void connected();

protected:
     QSqlQuery m_bookQuery;
     QList<int> m_titles;
};

#endif // TEXTBOOKREADER_H
