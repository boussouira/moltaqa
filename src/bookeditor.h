#ifndef BOOKEDITOR_H
#define BOOKEDITOR_H

#include "abstractbookreader.h"

class RichBookReader;
class QuaZipFile;

class BookEditor : public QObject
{
    Q_OBJECT

public:
    BookEditor(QObject *parent = 0);
    ~BookEditor();

    bool open(int bookID);
    bool open(LibraryBook::Ptr book);
    void setBookReader(RichBookReader *reader);

    void unZip();
    bool zip();
    bool save();
    void removeTemp();

    bool saveBookPages(QList<BookPage *> pages);
    void saveDom();
    void addPage(int pageID);
    void removePage();

    int maxPageID();
    QString titlesFile();

    void addPageLink(int sourcPage, int destBook, int destPage);
    void removePageLink(int sourcPage, int destBook, int destPage);

protected:
    bool zipDir(QString path, QuaZipFile &outFile);

protected:
    RichBookReader *m_bookReader;
    LibraryBook::Ptr m_book;
    bool m_removeReader;
    QString m_bookTmpDir;
    QString m_newBookPath;
    int m_lastBookID;
    QList<int> m_removedPages;
    QStringList m_createdDirs;
};

#endif // BOOKEDITOR_H
