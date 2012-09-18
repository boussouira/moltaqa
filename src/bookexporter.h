#ifndef BOOKEXPORTER_H
#define BOOKEXPORTER_H

#include <qobject.h>
#include "librarybook.h"

class BookExporter : public QObject
{
    Q_OBJECT
public:
    BookExporter(QObject *parent=0);

    void setLibraryBook(LibraryBookPtr book) { m_book = book; }
    QString genereatedPath() { return m_genereatedPath; }

    void setRemoveTashkil(bool remove) { m_removeTashkil = remove; }
    void setAddPageNumber(bool add) { m_addPageNumber = add; }

    virtual void start()=0;

protected:
    LibraryBookPtr m_book;
    QString m_genereatedPath;
    QString m_tempDir;
    bool m_removeTashkil;
    bool m_addPageNumber;
};

#endif // BOOKEXPORTER_H
