#ifndef RICHSIMPLEBOOKREADER_H
#define RICHSIMPLEBOOKREADER_H

#include "richbookreader.h"

class BookIndexNode;

class RichSimpleBookReader : public RichBookReader
{
public:
    RichSimpleBookReader(QObject *parent=0);
    ~RichSimpleBookReader();

    void goToPage(int pid = -1);
    void goToPage(int page, int part);
    void goToHaddit(int hadditNum);

    BookIndexModel *indexModel();
    BookIndexModel *topIndexModel();

protected:
    void childTitles(BookIndexNode *parentNode, int tid);
    void connected();
};

#endif // RICHSIMPLEBOOKREADER_H
