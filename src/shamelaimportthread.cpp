#include "shamelaimportthread.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "librarycreator.h"
#include "shamelaimportdialog.h"

ShamelaImportThread::ShamelaImportThread(QObject *parent) :
    QThread(parent)
{
    ShamelaImportDialog *importDialog = ShamelaImportDialog::importDialog();

    m_shamelaManager = importDialog->shamelaManager();
    m_shamelaInfo = importDialog->shamelaInfo();

    m_stop = false;
    m_importQuran = false;
}

void ShamelaImportThread::run()
{
    m_threadID = (int)currentThreadId();
    m_creator.setThreadID(m_threadID);
    m_creator.openDB();
    m_creator.setImportAuthors(ShamelaImportDialog::importDialog()->addAuthorsForEachBook());

    if(m_importQuran)
        importQuran();

    importBooks();
    emit doneImporting();
}

void ShamelaImportThread::stop()
{
    m_stop = true;
}

void ShamelaImportThread::importBooks()
{
    ShamelaBookInfo *book = m_shamelaManager->nextBook();
    while(book) {
        book->genInfo(m_shamelaInfo);
        m_creator.addBook(book);
        bookImported(book->name);

        delete book;

        if(m_stop) {
            break;
        }

        book = m_shamelaManager->nextBook();
    }
}

void ShamelaImportThread::importQuran()
{
    m_creator.addQuran();
    qDebug("[+] Quran Added");
}
