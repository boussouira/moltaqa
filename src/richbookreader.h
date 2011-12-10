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
      Check if this reader can have some books that need some time to load thier index model

      A fast index load mean that we should call first \ref topIndexModel and use
      threading to get the full index by calling \ref indexModel.
      @return True if it can take some time to load the full index model
      */
    virtual bool needFastIndexLoad();
    /**
      Get the full index model of the curren book
      */
    virtual BookIndexModel *indexModel() = 0;
    /**
      Get only a top level index model
      */
    virtual BookIndexModel *topIndexModel();

    TextFormatter *textFormat();

    void highlightPage(int pageID, lucene::search::Query *query);

    bool scrollToHighlight();

    void stopModelLoad();

    virtual int getPageTitleID(int pageID);

    void saveBookPage(QList<BookPage *> pages);

protected:
    virtual void connected();

signals:
    void textChanged();

protected:
    TextFormatter *m_textFormat;
    lucene::search::Query *m_query;
    int m_highlightPageID;
    bool m_stopModelLoad;
};

#endif // RICHBOOKREADER_H
