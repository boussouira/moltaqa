#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>

/**
  Generate a random file name with book extension
  @param path Books folder path
  @param fullPath if false return file name only, if true return the full path
*/

QString genBookName(QString path, bool fullPath=false);
int rand_int(int smin, int smax);

enum PULRAL{
    SECOND  = 1,
    MINUTE,
    HOUR,
    BOOK,
    AUTHOR,
    CATEGORIE
};

QString arPlural(int count, PULRAL word, bool html=false);


#define DB_OPEN_ERROR(path) qCritical("[%s:%d] Cannot open database at \"%s\".", \
                                __FILE__, \
                                __LINE__, \
                                qPrintable(path));

#define SQL_ERROR(error) qCritical("[%s:%d] SQL error: \"%s\".", \
                                          __FILE__, \
                                          __LINE__, \
                                          qPrintable(error));

#endif // COMMON_H
