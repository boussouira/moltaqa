#ifndef RICHQURANREADER_H
#define RICHQURANREADER_H

#include "richbookreader.h"

class QuranQuery;
class QuranTextFormat;

class RichQuranReader : public RichBookReader
{
public:
    RichQuranReader(QObject *parent=0);
    ~RichQuranReader();

    void goToPage(int pid = -1);
    void goToPage(int page, int part);
    void goToSora(int sora, int aya);
    void goToHaddit(int hadditNum);

    void nextPage();
    void prevPage();

    void nextAya();
    void prevAya();

    bool hasNext();
    bool hasPrev();

    bool needFastIndexLoad();

    QAbstractItemModel *indexModel();

protected:
    void connected();
    void getBookInfo();
    int getPageNumber(int soraNumber, int ayaNumber=1);
    int getSoraAyatCount(int sora);
    BookPage firstSoraAndAya(int page);

protected:
    QuranQuery *m_quranQuery;
    QuranTextFormat *m_formatter;
};

#endif // RICHQURANREADER_H
