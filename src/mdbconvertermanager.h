#ifndef MDBCONVERTERMANAGER_H
#define MDBCONVERTERMANAGER_H

#include "mdbconverter.h"
#include <qhash.h>

class MdbConverterManager
{
public:
    MdbConverterManager();
    ~MdbConverterManager();
    QString getConvertedDB(QString path);
    void deleteDB(QString path);

protected:
    QHash<QString, QPair<QString, int> > m_convertedFiles;
};

#endif // MDBCONVERTERMANAGER_H
