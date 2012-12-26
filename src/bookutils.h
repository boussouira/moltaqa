#ifndef BOOKUTILS_H
#define BOOKUTILS_H

#include <qstring.h>
#include <qlist.h>

namespace Utils {
namespace Book {

int getPageTitleID(QList<int> &titles, int pageID);

bool hasShamelaShoorts(const QString &text);
QString fixShamelaShoorts(QString text);

}
}

#endif // BOOKUTILS_H
