#ifndef COMMON_H
#define COMMON_H

#include <qdir.h>
#include <qfile.h>

/**
  Generate a random file name with book extension
  @param path Books folder path
*/

QString genBookName(QString path);
int rand_int(int smin, int smax);

#endif // COMMON_H
