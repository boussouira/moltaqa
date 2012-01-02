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
};

}

#endif // SQLUTILS_H
