#ifndef ZIPUTILS_H
#define ZIPUTILS_H

#include <qstring.h>

class QuaZip;

namespace Utils
{

namespace Zip {
bool unzip(const QString &zipPath, const QString &outPath);
bool zip(const QString &dir, const QString &zipPath);
bool copyFromZip(const QString &sourcZipPath, QuaZip *targetZip);
}

}

#endif // ZIPUTILS_H
