#ifndef RICHBOOKREADER_H
#define RICHBOOKREADER_H

#include "abstractdbhandler.h"

class RichBookReader : public AbstractBookReader
{
    Q_OBJECT

public:
    RichBookReader(QObject *parent=0);

    /**
      Check if this handler can have some books that need some time to load thier index model

      A fast index load mean that we should call first \ref topIndexModel and use
      threading to get the full index by calling \ref indexModel.
      @return True if it can take some time to load the full index model
      */
    virtual bool needFastIndexLoad();
    /**
      Get the full index model of the curren book
      */
    virtual QAbstractItemModel *indexModel() = 0;
    /**
      Get only a top level index model
      */
    virtual QAbstractItemModel *topIndexModel();

    QString text();

signals:
    void textChanged();

protected:
    TextFormatter *m_textFormat;
};

#endif // RICHBOOKREADER_H
