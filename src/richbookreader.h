#ifndef RICHBOOKREADER_H
#define RICHBOOKREADER_H

#include "abstractbookreader.h"

class LibraryBookManager;
class QStandardItemModel;

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

    void stopModelLoad();

    TextFormatter *textFormat();

    bool scrollToHighlight();

    int getPageTitleID(int pageID);

    void setSaveReadingHistory(bool save);

protected:
    virtual void connected();
    virtual QString proccessPageText(QString text);

protected slots:
    void updateHistory();

signals:
    void textChanged();

protected:
    TextFormatter *m_textFormat;
    QStandardItemModel *m_indexModel;
    LibraryBookManager *m_bookmanager;
    QList<int> m_pageTitles;
    bool m_saveReadingHistory;
    bool m_stopModelLoad;
};

#endif // RICHBOOKREADER_H
