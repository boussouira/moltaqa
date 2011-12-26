#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"
#include "simplequery.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    /**
      Get current page text
      */
    virtual QString text()=0;

    virtual void getTitles();

protected:
     void connected();

protected:
     QSqlQuery m_bookQuery;
     QString m_text;
     QList<int> m_titles;
};

#endif // TEXTBOOKREADER_H
