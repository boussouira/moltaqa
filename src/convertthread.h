#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <qthread.h>
#include <qstringlist.h>
#include "authorinfo.h"

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
    void setProgress(int prog);

protected:
    ImportModel *m_model;
    LibraryManager *m_libraryManager;
    BookListManager *m_bookListManager;
    AuthorsManager *m_authorsManager;
    QStringList m_files;
    QString m_tempDB;
    int m_convertTime;
    int m_convertedFiles;
};

#endif // CONVERTTHREAD_H
