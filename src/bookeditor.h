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
    void setBookReader(RichBookReader *reader);

    void unZip();
    bool zip();
    bool save();
    
    bool saveBookPages(QList<BookPage *> pages);

protected:
    bool zipDir(QString path, QuaZipFile *outFile);

protected:
    RichBookReader *m_bookReader;
    LibraryBook *m_book;
    QString m_bookTmpDir;
    QString m_newBookPath;
    int m_lastBookID;
};

#endif // BOOKEDITOR_H
