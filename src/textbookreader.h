#ifndef TEXTBOOKREADER_H
#define TEXTBOOKREADER_H

#include "abstractbookreader.h"

class TextBookReader : public AbstractBookReader
{

public:
    TextBookReader(QObject *parent = 0);
    ~TextBookReader();

    void firstPage();

    virtual void load();

protected:
    virtual void loadTitles();

protected:
     QList<int> m_titles;
     QHash<int, QString> m_titlesText;
     bool m_loadTitlesText;
};

#endif // TEXTBOOKREADER_H
