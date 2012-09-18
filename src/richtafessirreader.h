#ifndef RICHTAFESSIRREADER_H
#define RICHTAFESSIRREADER_H

#include "richbookreader.h"

class TafessirTextFormat;

class RichTafessirReader : public RichBookReader
{
public:
    RichTafessirReader(QObject *parent=0);
    ~RichTafessirReader();

    void setShowQuranText(bool show);

    void setCurrentPage(QDomElement pageNode);
    QDomElement getQuranPageId(int sora, int aya);

protected:
    void connected();
    void openQuranBook();
    void readQuranText(int sora, int aya, int count);
    int nextAyaNumber(QDomElement e);

protected:
    TafessirTextFormat *m_formatter;
    LibraryBookPtr m_quranInfo;
    XmlDomHelper m_quranDom;
    bool m_showQuran;
};

#endif // RICHTAFESSIRREADER_H
