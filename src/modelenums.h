#ifndef MODELENUMS_H
#define MODELENUMS_H

#include <qstandarditemmodel.h>


namespace ItemRole {
    enum {
        idRole = Qt::UserRole + 2,
        typeRole,
        soraRole,
        ayaRole
    };
}

namespace ItemType {
    enum {
        BookItem = 1,
        CategorieItem,
        AuthorItem
    };
}

#endif // MODELENUMS_H
