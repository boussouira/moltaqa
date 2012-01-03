#ifndef RICHTAFESSIRREADER_H
#define RICHTAFESSIRREADER_H

#include "richbookreader.h"

class BookIndexNode;
class TafessirTextFormat;

class RichTafessirReader : public RichBookReader
{
public:
    RichTafessirReader(QObject *parent=0, bool showQuran=true);
    ~RichTafessirReader();

    void setCurrentPage(QDomElement pageNode);
    QDomElement getQuranPageId(int sora, int aya);

protected:
    void childTitles(BookIndexNode *parentNode, int tid);
    void connected();
    void openQuranBook();
    void readQuranText(int sora, int aya, int count);

protected:
    TafessirTextFormat *m_formatter;
    QSqlDatabase m_quranDB;
    LibraryBook *m_quranInfo;
    bool m_showQuran;
};

#endif // RICHTAFESSIRREADER_H
