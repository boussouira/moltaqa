#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <qobject.h>
#include <qcoreapplication.h>
#include "librarybook.h"

class LibraryInfo
{
    Q_DECLARE_TR_FUNCTIONS(ConnectionInfo)

public:
    /**
        Load information from an info.xml file
        @param booksPath Path where info.xml exsists
    */
    LibraryInfo();

    void load(const QString &path);

    QString name();
    QString path();

    /**
     Get data directory
     */

    QString dataDir();

    /**
      Get full path to books directory
      */
    QString booksDir();

    /**
      Get temporary folder path
      */
    QString tempDir();

    /**
      Index direcrory
      */
    QString indexDir();

    /**
      CLucene index path
      */
    QString indexDataDir();

    /**
      Path to the index tracker file
      */
    QString trackerFile();

    void setName(const QString &name);
    void setPath(const QString &path);
    void setBooksDir(const QString &dir);

    /**
      Get book path
      @note This function doesn't check if the file exists
      @param bookName The name of book file
      */
    QString bookPath(QString fileName);
    QString bookPath(LibraryBook::Ptr book);

protected:
    void checkDataFiles(const QString &dataDirPath);

protected:
    QString m_name;         ///< Library name
    QString m_path;         ///< Library path
    QString m_dataDir;
    QString m_booksDir;     ///< Absolute Books path
    QString m_tempsDir;     ///< Absolute temp path
    QString m_indexDir;
    QString m_indexDataDir;
    QString m_trackerFile;
};

#endif // CONNECTIONINFO_H
