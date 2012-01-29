#ifndef RICHTAFESSIRREADER_H
#define RICHTAFESSIRREADER_H

#include "richbookreader.h"

class TafessirTextFormat;

class RichTafessirReader : public RichBookReader
{
public:
    RichTafessirReader(QObject *parent=0, bool showQuran=true);
    ~RichTafessirReader();

    void setCurrentPage(QDomElement pageNode);
    QDomElement getQuranPageId(int sora, int aya);

protected:
    void connected();
    void openQuranBook();
    void readQuranText(int sora, int aya, int count);
    int nextAyaNumber(QDomElement e);

protected:
    TafessirTextFormat *m_formatter;
    QuaZip m_quranZip;
    LibraryBook *m_quranInfo;
    QuaZipFile m_quranPages;
    QDomDocument m_qurankDoc;
    QDomElement m_quranRootElement;
    bool m_showQuran;
};

#endif // RICHTAFESSIRREADER_H
