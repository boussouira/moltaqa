#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include "authorinfo.h"

#include <qstringlist.h>
#include <qthread.h>

class ImportModel;
class ImportModelNode;
class LibraryManager;
class BookListManager;
class AuthorsManager;
class QSqlDatabase;
class QuaZip;
class QDomElement;

class ConvertThread : public QThread
{
    Q_OBJECT

public:
    ConvertThread(QObject *parent = 0);
    void setFiles(QStringList &list) { m_files = list;}
    void setModel(ImportModel *model) { m_model = model;}
    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}
    int convertTime() { return m_convertTime; }
    int convertedFiles() { return m_convertedFiles; }
    void run();

protected:
    void convert(const QString &path);

    void convertShamelaBook(const QString &path);
    /**
      Copy book with BookID from bookDB to a temporary file and set it as the book path
      */
    void copyBookFromShamelaBook(ImportModelNode *node, const QSqlDatabase &bookDB, int bookID);
    QString getBookType(const QSqlDatabase &bookDB);

    void convertMoltaqaPackage(const QString &path);
    void extractMoltaqaBook(QuaZip &zip, QDomElement &bookElement, QDomElement &authorsElement);
    AuthorInfo::Ptr importAuthorInfo(QDomElement &authorElement);

signals:
    void bookConverted(QString bookName);
    void addBooksToProgress(int count);

protected:
    ImportModel *m_model;
    LibraryManager *m_libraryManager;
    BookListManager *m_bookListManager;
    AuthorsManager *m_authorsManager;
    QStringList m_files;
    int m_convertTime;
    int m_convertedFiles;
};

#endif // CONVERTTHREAD_H
