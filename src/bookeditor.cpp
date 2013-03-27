#include "bookeditor.h"
#include "richbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "librarybookmanager.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "bookexception.h"

BookEditor::BookEditor(QObject *parent) :
    QObject(parent)
{
    m_lastBookID = 0;
    m_bookReader = 0;
    m_removeReader = false;
    m_needUnZip = true;
}

BookEditor::~BookEditor()
{
    if(m_removeReader) {
        ml_delete_check(m_bookReader);
    }
}

bool BookEditor::open(int bookID)
{
    return open(LibraryManager::instance()->bookManager()->getLibraryBook(bookID));
}

bool BookEditor::open(LibraryBook::Ptr book)
{
    if(book) {
        RichBookReader *bookReader;
        if(book->isNormal()) {
            bookReader = new RichSimpleBookReader();
        } else if(book->isTafessir()) {
            bookReader = new RichTafessirReader();
        } else {
            qDebug() << "BookEditor::open Can't edit book:" << book->path;
            return false;
        }

        bookReader->setBookInfo(book);

        try {
            bookReader->openBook();

            m_bookReader = bookReader;
            m_book = book;

            if(m_book->id != m_lastBookID)
                m_bookTmpDir.clear();

            m_removeReader = true;
            return true;

        } catch (BookException &) {
            delete bookReader;
            return false;
        }
    } else {
        return false;
    }
}

void BookEditor::setBookReader(RichBookReader *reader)
{
    m_bookReader = reader;
    m_book = reader->book();

    if(m_book->id != m_lastBookID) {
        m_needUnZip = true;
        m_zipHelper = ZipHelper();
    }

    m_removeReader = false;
}

ZipHelper *BookEditor::zipHelper()
{
    return &m_zipHelper;
}

void BookEditor::unZip()
{
    if(!m_needUnZip && QFile::exists(m_zipHelper.datbasePath()))
        return;

    m_zipHelper.open();
    m_zipHelper.addFromZip(m_book->path);

    m_needUnZip = false;

    m_lastBookID = m_book->id;
}

bool BookEditor::zip()
{
    m_newBookPath = m_zipHelper.zip();

    return m_newBookPath.size();
}

bool BookEditor::save()
{
    // Delete Existing backup
    QString backupFile = m_book->path + ".back";

    if(QFile::exists(backupFile))
        QFile::remove(backupFile);

    if(m_removeReader) {
        ml_delete_check(m_bookReader);
    }

    // Create a new backup
    if(QFile::copy(m_book->path, backupFile)) {
        QFile::remove(m_book->path);
    } else {
        qWarning() << "BookEditor::save Can't make a backup for:" << m_book->path;
        return false;
    }

    // Copy new book
    if(QFile::copy(m_newBookPath, m_book->path)) {
        QFile::remove(m_newBookPath);
        m_newBookPath.clear();
    } else {
        qWarning() << "BookEditor::save Can't copy" << m_newBookPath << "to" << m_book->path;
        return false;
    }

    return true;
}

bool BookEditor::saveBookPages(QList<BookPage*> pages)
{
    foreach(BookPage *page, pages) {
        QString fileName = QString("pages/p%2.html").arg(page->pageID);
        if(m_removedPages.contains(page->pageID)) {
            m_zipHelper.remove(fileName);
            m_removedPages.removeAll(page->pageID);

            continue;
        }

        QDomElement e = m_bookReader->pagesDom().findElement("id", QString::number(page->pageID));
        if(!e.isNull()) {
            e.setAttribute("page", page->page);
            e.setAttribute("part", page->part);

            if(page->haddit)
                e.setAttribute("haddit", page->haddit);

            if(!m_book->isNormal()) {
                e.setAttribute("sora", page->sora);
                e.setAttribute("aya", page->aya);
            }
        } else {
            qDebug("BookEditor::saveBookPages no element with id %d", page->pageID);
        }

        m_zipHelper.update(page);
    }

    m_zipHelper.replaceFromDomHelper("pages.xml",
                                     m_bookReader->pagesDom(),
                                     ZipHelper::Top);

    return true;
}

void BookEditor::saveDom()
{
    m_zipHelper.replaceFromDomHelper("pages.xml",
                                     m_bookReader->pagesDom(),
                                     ZipHelper::Top);
}

void BookEditor::addPage(int pageID, bool insertAfterCurrent)
{
    QDomElement e = m_bookReader->pagesDom().currentElement();
    QDomElement page = m_bookReader->pagesDom().domDocument().createElement("page");

    page.setAttribute("id", pageID);
    page.setAttribute("page", e.attribute("page"));
    page.setAttribute("part", e.attribute("part"));

    if(m_book->isTafessir()) {
        page.setAttribute("aya", e.attribute("aya"));
        page.setAttribute("sora", e.attribute("sora"));
    }

    page.setAttribute("tid", m_bookReader->getPageTitleID(e.attribute("id").toInt()));

    QDomElement newPage;
    if(insertAfterCurrent)
        newPage = m_bookReader->pagesDom().rootElement().insertAfter(page, e).toElement();
    else
        newPage = m_bookReader->pagesDom().rootElement().insertBefore(page, e).toElement();

    if(!newPage.isNull())
        m_bookReader->goToPage(pageID);
}

void BookEditor::removePage()
{
    QDomElement page = m_bookReader->pagesDom().currentElement();

    if(m_bookReader->hasNext())
        m_bookReader->nextPage();
    else
        m_bookReader->prevPage();

    QDomElement removedPage = m_bookReader->pagesDom().rootElement().removeChild(page).toElement();
    if(!removedPage.isNull())
        m_removedPages.append(removedPage.attribute("id").toInt());
}

int BookEditor::maxPageID()
{
    int pageID = 0;
    QDomElement e = m_bookReader->pagesDom().rootElement().firstChildElement();
    while(!e.isNull()) {
        int pid = e.attribute("id").toInt();

        if(pid > pageID)
            pageID = pid;

        e = e.nextSiblingElement();
    }

    return pageID;
}

void BookEditor::addPageLink(int sourcPage, QString destBookUUID, int destPage)
{
    QDomElement pageElement = m_bookReader->pagesDom().findElement("id", sourcPage);
    if(!pageElement.isNull()) {
        QDomElement linkElement = m_bookReader->pagesDom().domDocument().createElement("link");
        linkElement.setAttribute("book", destBookUUID);
        linkElement.setAttribute("page", destPage);

        pageElement.appendChild(linkElement);
    }
}

void BookEditor::removePageLink(int sourcPage, QString destBookUUID, int destPage)
{
    QDomElement pageElement = m_bookReader->pagesDom().findElement("id", sourcPage);
    if(!pageElement.isNull()) {
        QDomElement linkElement = pageElement.firstChildElement("link");
        while(!pageElement.isNull()) {
            if(linkElement.attribute("book") == destBookUUID
                    && linkElement.attribute("page").toInt() == destPage) {
                pageElement.removeChild(linkElement);
                break;
            }

            linkElement = linkElement.nextSiblingElement("link");
        }
    }
}
