#ifndef RICHBOOKREADER_H
#define RICHBOOKREADER_H

#include "abstractbookreader.h"
#include "clutils.h"
#include "clucenequery.h"

class LibraryBookManager;

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

    void highlightPage(int pageID, CLuceneQuery *query);

    bool scrollToHighlight();

    void getPageTitleID();
    int getPageTitleID(int pageID);

protected:
    virtual void connected();
    void readItem(QDomElement &element, QStandardItem *parent);

protected slots:
    void updateHistory();

signals:
    void textChanged();

protected:
    TextFormatter *m_textFormat;
    CLuceneQuery *m_query;
    LibraryBookManager *m_bookmanager;
    QList<int> m_pageTitles;
    int m_highlightPageID;
    bool m_removeTashekil;
};

#endif // RICHBOOKREADER_H
