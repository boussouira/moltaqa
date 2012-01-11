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
    int getPageTitleID(int pageID);
    virtual void getTitles();
    virtual void getPages();

protected:
     QList<int> m_titles;
     QHash<int, QByteArray> m_pages;
};

#endif // TEXTBOOKREADER_H
