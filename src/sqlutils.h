#ifndef SQLUTILS_H
#define SQLUTILS_H

#include <qstring.h>

namespace Utils {

class DatabaseRemover
{
public:
    DatabaseRemover();
    ~DatabaseRemover();

    QString connectionName;
    int removeModel;
};

}

#endif // SQLUTILS_H
