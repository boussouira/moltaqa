#ifndef MODELENUMS_H
#define MODELENUMS_H

#include <qstandarditemmodel.h>

namespace ItemRole {
    enum {
        idRole = Qt::UserRole + 2,
        typeRole,
        soraRole,
        ayaRole,
        orderRole,
        authorDeathRole,
        authorIdRole,
        itemTypeRole,
        bookIdRole,
        pageIdRole
    };
}

namespace ItemType {
    enum {
        BookItem = 1,
        CategorieItem
    };
}

#endif // MODELENUMS_H
