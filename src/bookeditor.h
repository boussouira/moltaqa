#ifndef BOOKEDITOR_H
#define BOOKEDITOR_H

#include <qobject.h>
#include "librarybook.h"
#include "ziphelper.h"

class RichBookReader;
class QuaZipFile;
class ZipHelper;

class BookEditor : public QObject
{
    Q_OBJECT

public:
    BookEditor(QObject *parent = 0);
    ~BookEditor();

    bool open(int bookID);
    bool open(LibraryBook::Ptr book);
    void setBookReader(RichBookReader *reader);

    ZipHelper *zipHelper();

    void unZip();
    bool zip();
    bool save();

    bool saveBookPages(QList<BookPage *> pages);
    void saveDom();
    void addPage(int pageID, bool insertAfterCurrent);
    void removePage();

    int maxPageID();

    void addPageLink(int sourcPage, QString destBookUUID, int destPage);
    void removePageLink(int sourcPage, QString destBookUUID, int destPage);

protected:
    RichBookReader *m_bookReader;
    LibraryBook::Ptr m_book;
    ZipHelper m_zipHelper;
    bool m_needUnZip;
    bool m_removeReader;
    QString m_bookTmpDir;
    QString m_newBookPath;
    int m_lastBookID;
    QList<int> m_removedPages;
    QStringList m_createdDirs;
};

#endif // BOOKEDITOR_H
