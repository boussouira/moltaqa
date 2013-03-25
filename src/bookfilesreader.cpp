#include "bookfilesreader.h"
#include "utils.h"

#include <qfile.h>
#include <quazipfile.h>

#define PAGE_FILE_PREFIX "pages/p"
#define PAGE_FILE_SUFFIX ".html"

BookFilesReader::BookFilesReader()
{
}

LibraryBook::Ptr BookFilesReader::book() const
{
    return m_book;
}

void BookFilesReader::setBook(const LibraryBook::Ptr &book)
{
    m_book = book;
}

void BookFilesReader::start()
{
    bool closeZip = false;
    if(!m_zip.isOpen()) {
        open();
        closeZip = true;
    }

    QuaZipFileInfo info;
    QuaZipFile file(&m_zip);
    for(bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile()) {
        ml_return_on_fail2(m_zip.getCurrentFileInfo(&info),
                           "BookFilesReader::start getCurrentFileInfo Error" << m_zip.getZipError());

        QString filePath = info.name.toLower();
        if(!acceptFile(filePath))
            continue;

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("BookFilesReader::start zip error %d", m_zip.getZipError());
            continue;
        }

        readFile(filePath, file);

        file.close();
    }

    if(closeZip)
        close();
}

void BookFilesReader::open()
{
    ml_return_on_fail2(m_book, "BookFilesReader::open book is null");

    ml_return_on_fail2(QFile::exists(m_book->path),
                       "BookFilesReader::open file doesn't exists" << m_book->path);

    close();

    m_zip.setZipName(m_book->path);

    ml_return_on_fail2(m_zip.open(QuaZip::mdUnzip),
                       "BookFilesReader::open Can't zip file" << m_book->path
                       << "Error" << m_zip.getZipError());

}

void BookFilesReader::close()
{
    if(m_zip.isOpen())
        m_zip.close();
}

bool BookFilesReader::acceptFile(QString filePath)
{
    return filePath.startsWith(PAGE_FILE_PREFIX);
}

bool BookFilesReader::readFile(QString filePath, QIODevice &file)
{
    QByteArray out;
    Utils::Files::copyData(file, out);

    QString name = filePath;
    if(name.startsWith(PAGE_FILE_PREFIX)) {
        name = name.remove(0, 7);
        name = name.remove(PAGE_FILE_SUFFIX);

        bool ok;
        int pageID = name.toInt(&ok);
        if(ok)
            readPageText(pageID, filePath, QString::fromUtf8(out));
    }

    return true;
}

void BookFilesReader::readPageText(int pageID, QString filePath, QString text)
{
    Q_UNUSED(pageID)
    Q_UNUSED(filePath)
    Q_UNUSED(text)
}
