#include "shemehandler.h"
#include "utils.h"
#include "librarymanager.h"
#include "bookreaderview.h"
#include "librarybookmanager.h"
#include "bookwidget.h"
#include "authorsview.h"
#include "tarajemrowatview.h"
#include "mainwindow.h"
#include <qurlquery.h>

ShemeHandler::ShemeHandler()
{
}

void ShemeHandler::open(const QUrl &url)
{
    QString command = url.queryItemValue("c").toLower();

    if(command == "open") {
        commandOpen(url);
    } else {
        qWarning() << "ShemeHandler::open unknow command:" << command;
    }
}

void ShemeHandler::commandOpen(const QUrl &url)
{
    QString type = url.queryItemValue("t").toLower();
    if(type == "book") {
        openBook(url);
    } else if(type == "quran") {
        openQuran(url);
    } else if(type == "author") {
        openAuthor(url);
    } else if(type == "rawi") {
        openRawi(url);
    } else {
        qWarning() << "ShemeHandler::commandOpen unknow type:" << type;
    }
}

void ShemeHandler::openBook(const QUrl &url)
{
    QUrlQuery urlQuery(url);
    ml_return_on_fail2(urlQuery.hasQueryItem("id"), "MoltaqaShemeHandler::openBook url doesn't have an id query item");

    QString bookUUID = urlQuery.queryItemValue("id");
    int page = urlQuery.hasQueryItem("page") ? urlQuery.queryItemValue("page").toInt() : -1;

    LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookUUID);
    ml_return_on_fail2(book, "MoltaqaShemeHandler::openBook can't find book:" << qPrintable(bookUUID));

    MW->bookReaderView()->openBook(book->id, page);
}

void ShemeHandler::openQuran(const QUrl &url)
{
    QUrlQuery urlQuery(url);
    ml_return_on_fail2(urlQuery.hasQueryItem("sora"), "MoltaqaShemeHandler::openQuran url doesn't have a sora query item");

    int sora = urlQuery.queryItemValue("sora").toInt();
    int aya = urlQuery.hasQueryItem("aya") ? urlQuery.queryItemValue("aya").toInt() : 1;

    LibraryBook::Ptr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
    ml_return_on_fail2(quranBook, "MoltaqaShemeHandler::openQuran quranBook is null");

    BookViewBase *w = MW->bookReaderView()->openBook(quranBook->id);
    ml_return_on_fail2(w, "MoltaqaShemeHandler::openQuran quranBook is null");

    w->openSora(sora, aya);
}

void ShemeHandler::openAuthor(const QUrl &url)
{
    QUrlQuery urlQuery(url);
    ml_return_on_fail2(urlQuery.hasQueryItem("id"), "MoltaqaShemeHandler::openAuthor url doesn't have an id query item");
    int author = urlQuery.queryItemValue("id").toInt();

    AuthorsView::instance()->openAuthorInfo(author);
}

void ShemeHandler::openRawi(const QUrl &url)
{
    QUrlQuery urlQuery(url);
    ml_return_on_fail2(urlQuery.hasQueryItem("id"), "MoltaqaShemeHandler::openRawi url doesn't have an id query item");
    int rawi = urlQuery.queryItemValue("id").toInt();

    TarajemRowatView::instance()->openRawiInfo(rawi);
}
