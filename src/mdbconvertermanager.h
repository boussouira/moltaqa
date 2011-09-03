#ifndef MDBCONVERTERMANAGER_H
#define MDBCONVERTERMANAGER_H

#ifdef USE_MDBTOOLS
    #include "mdbconverter.h"
#endif

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
