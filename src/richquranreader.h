#ifndef RICHQURANREADER_H
#define RICHQURANREADER_H

#include "richbookreader.h"

class QuranTextFormat;

class RichQuranReader : public RichBookReader
{
public:
    RichQuranReader(QObject *parent=0);
    ~RichQuranReader();

    void setCurrentPage(QDomElement pageNode);
    BookIndexModel *indexModel();

    void nextPage();
    void prevPage();

    void nextAya();
    void prevAya();

protected:
    QuranTextFormat *m_formatter;
};

#endif // RICHQURANREADER_H
