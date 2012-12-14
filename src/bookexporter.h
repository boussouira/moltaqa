#ifndef BOOKEXPORTER_H
#define BOOKEXPORTER_H

#include <qobject.h>
#include "librarybook.h"

class BookExporter : public QObject
{
    Q_OBJECT
public:
    BookExporter(QObject *parent=0);

    void setExportInOnePackage(bool onePackage) { m_exportInOnePackage = onePackage; }
    void setLibraryBook(LibraryBookPtr book) { m_book = book; }
    void setLibraryBookList(QList<LibraryBookPtr> bookList) { m_bookList = bookList; }
    QString genereatedPath() { return m_genereatedPath; }

    void setRemoveTashkil(bool remove) { m_removeTashkil = remove; }
    void setAddPageNumber(bool add) { m_addPageNumber = add; }

    void stop() { m_stop = true; }

    int exportedBooksCount() { return m_exportedBooks; }
    /**
     * @brief Export multi book in one package
     * @return true if multi book export is supported
     */
    virtual bool multiBookExport()=0;

    virtual void start()=0;

signals:
    void bookExported(QString book);

protected:
    LibraryBookPtr m_book; ///< current book to export
    QList<LibraryBookPtr> m_bookList; ///< list of books to export in one package, only supported by our format
    QString m_genereatedPath;
    QString m_tempDir;
    int m_exportedBooks;
    bool m_stop;
    bool m_exportInOnePackage;
    bool m_removeTashkil;
    bool m_addPageNumber;
};

#endif // BOOKEXPORTER_H
