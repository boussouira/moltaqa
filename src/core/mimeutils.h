#ifndef MIMEUTILS_H
#define MIMEUTILS_H

#include <qstring.h>

namespace Utils {
namespace Mimes {

QString fileTypeFromExt(QString ext);
QString fileTypeFromFileName(QString filename);

}
}

#endif // MIMEUTILS_H
