#ifndef RICHSIMPLEBOOKREADER_H
#define RICHSIMPLEBOOKREADER_H

#include "richbookreader.h"

class RichSimpleBookReader : public RichBookReader
{
public:
    RichSimpleBookReader(QObject *parent=0);
    ~RichSimpleBookReader();

    void setCurrentPage(QDomElement pageNode);

protected:
    void connected();
};

#endif // RICHSIMPLEBOOKREADER_H
