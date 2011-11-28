#ifndef TEXTTAFESSIRREADER_H
#define TEXTTAFESSIRREADER_H

#include "textbookreader.h"

class TextTafessirReader : public TextBookReader
{
    Q_OBJECT
public:
    TextTafessirReader(QObject *parent = 0);
    ~TextTafessirReader();
    
    void getTitles();
    void goFirst();

    void goToPage(int pid);
    void goToPage(int page, int part);

    bool hasPrev();
    bool hasNext();

    void nextPage();

    QString text();

protected:
    QHash<int, int> m_titles;
};

#endif // TEXTTAFESSIRREADER_H
