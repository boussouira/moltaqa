#ifndef RICHSIMPLEBOOKREADER_H
#define RICHSIMPLEBOOKREADER_H

#include "richbookreader.h"

class RichSimpleBookReader : public RichBookReader
{
public:
    RichSimpleBookReader(QObject *parent=0);
    ~RichSimpleBookReader();

    void setShowShorooh(bool show);

    void setCurrentPage(QDomElement pageNode);
    void getShorooh();

protected:
    void connected();

protected:
    bool m_showShorooh;
};

#endif // RICHSIMPLEBOOKREADER_H
