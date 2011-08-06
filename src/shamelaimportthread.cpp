#include "shamelaimportthread.h"
#include "shamelainfo.h"
#include "shamelamanager.h"
#include "librarycreator.h"

ShamelaImportThread::ShamelaImportThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}

void ShamelaImportThread::run()
{
    m_creator.m_library = m_library;
    m_creator.openDB();
    m_creator.createTables();

    emit setStepsRange(0, 3);
    emit setStep(0);


    m_creator.start();
    importCats();
    importAuthors();
    m_creator.done();

    m_creator.start();
    importBooks();
    m_creator.done();

    emit debugInfo(tr("انتهى اسيراد الكتب بنجاح"));
    emit doneImporting();
}

void ShamelaImportThread::stop()
{
    m_stop = true;
}

void ShamelaImportThread::importCats()
{
    emit stepTitle(tr("استيراد الاقسام:"));
    emit debugInfo(tr("جاري استيراد الاقسام..."));

    int catsCount = m_manager->getCatCount();
    int prog = 0;

    emit setRange(0, catsCount);
    emit setProgress(0);

    m_manager->selectCats();
    CategorieInfo *cat = m_manager->nextCat();
    while(cat) {
        setProgress(++prog);
        m_creator.addCat(cat);

        delete cat;
        cat = m_manager->nextCat();
    }

    emit setStep(1);
}

void ShamelaImportThread::importAuthors()
{
    emit stepTitle(tr("استيراد المؤلفيين:"));
    emit debugInfo(tr("جاري اضافة المؤلفيين..."));

    int catsCount = m_manager->getAuthorsCount();
    int prog = 0;

    emit setRange(0, catsCount);
    emit setProgress(0);

    m_manager->selectAuthors();
    AuthorInfo *auth = m_manager->nextAuthor();
    while(auth) {
        setProgress(++prog);
        m_creator.addAuthor(auth);

        delete auth;
        auth = m_manager->nextAuthor();
    }

    emit setStep(2);
}

void ShamelaImportThread::importBooks()
{
    emit stepTitle(tr("استيراد الكتب:"));
    emit debugInfo(tr("بدأ استيراد الكتب..."));

    int booksCount = m_manager->getBooksCount();
    int prog = 0;

    emit setRange(0, booksCount);
    emit setProgress(0);

    m_manager->selectBooks();
    ShamelaBookInfo *book = m_manager->nextBook();
    while(book) {
        setProgress(++prog);
        book->genInfo(m_shamela);
        m_creator.addBook(book);

        emit debugInfo(QString(" + ")+book->name);
        delete book;

        if(m_stop) {
            emit debugInfo(tr("جاري ايقاف الفهرسة..."));
            break;
        }

        book = m_manager->nextBook();
    }

    emit setStep(3);
}
