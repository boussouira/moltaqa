#ifndef RICHBOOKREADER_H
#define RICHBOOKREADER_H

#include "abstractbookreader.h"
#include "clutils.h"

class RichBookReader : public AbstractBookReader
{
    Q_OBJECT

public:
    RichBookReader(QObject *parent=0);
    ~RichBookReader();

    /**
      Get the full index model of the curren book
      */
    virtual QStandardItemModel *indexModel();

    TextFormatter *textFormat();

    void highlightPage(int pageID, lucene::search::Query *query);

    bool scrollToHighlight();

    void getPageTitleID();
    int getPageTitleID(int pageID);

protected:
    virtual void connected();
    void readItem(QDomElement &element, QStandardItem *parent);

signals:
    void textChanged();

protected:
    TextFormatter *m_textFormat;
    lucene::search::Query *m_query;
    QList<int> m_pageTitles;
    int m_highlightPageID;
};

#endif // RICHBOOKREADER_H
