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
        pageIdRole,
        titleIdRole,
        uuidRole,
        newBookRole,
        fileNameRole
    };
}

namespace ItemType {
    enum {
        BookItem = 1,
        CategorieItem
    };
}

namespace ModelSort {
    enum {
        OrderSort,
        BookTitleSort,
        AuthorNameSort,
        AuthorDeathSort
    };
};

#endif // MODELENUMS_H
