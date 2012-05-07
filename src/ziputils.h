#ifndef ZIPUTILS_H
#define ZIPUTILS_H

#include <qstring.h>

namespace Utils
{

namespace Zip {
bool unzip(const QString &zipPath, const QString &outPath);
bool zip(const QString &dir, const QString &zipPath);
}

}

#endif // ZIPUTILS_H
