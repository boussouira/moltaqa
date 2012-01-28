#ifndef MODELUTILS_H
#define MODELUTILS_H

#include <qabstractitemmodel.h>

namespace Utils {

QModelIndex findModelIndex(QAbstractItemModel *model, int tid);

}

#endif // MODELUTILS_H
