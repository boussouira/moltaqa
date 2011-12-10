#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"
#include "simplequery.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    virtual void goFirst()=0;

    /**
      Get current page text
      */
    virtual QString text() = 0;

protected:
     void connected();

protected:
     QSqlQuery m_bookQuery;
     QString m_text;
};

#endif // TEXTBOOKREADER_H
