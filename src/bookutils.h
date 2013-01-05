#ifndef BOOKUTILS_H
#define BOOKUTILS_H

#include <qstring.h>
#include <qlist.h>

class QSqlDatabase;

namespace Utils {
namespace Book {

int getPageTitleID(QList<int> &titles, int pageID);

bool hasShamelaShoorts(const QString &text);
QString fixShamelaShoorts(QString text);

QString shamelaQueryFields(const QSqlDatabase &db, QString tableName);
}
}

#endif // BOOKUTILS_H
