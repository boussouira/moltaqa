#ifndef SIMPLEDBHANDLER_H
#define SIMPLEDBHANDLER_H

#include "richbookreader.h"

class BookIndexNode;
class SimpleQuery;

class RichSimpleBookReader : public RichBookReader
{
public:
    RichSimpleBookReader(QObject *parent=0);
    ~RichSimpleBookReader();

    void goToPage(int pid = -1);
    void goToPage(int page, int part);
    void goToSora(int sora, int aya);
    void goToHaddit(int hadditNum);

    void nextPage();
    void prevPage();

    bool hasNext();
    bool hasPrev();

    QAbstractItemModel *indexModel();
    QAbstractItemModel *topIndexModel();

protected:
    void getBookInfo();
    void childTitles(BookIndexNode *parentNode, int tid);
    void connected();

protected:
    SimpleQuery *m_simpleQuery;
};

#endif // SIMPLEDBHANDLER_H
