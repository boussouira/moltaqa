#include "shemehandler.h"
#include "utils.h"
#include "librarymanager.h"
#include "booksviewer.h"
#include "librarybookmanager.h"
#include "bookwidget.h"
#include "authorsview.h"
#include "tarajemrowatview.h"

ShemeHandler::ShemeHandler()
{
}

void ShemeHandler::open(const QUrl &url)
{
    QString command = url.host().toLower();

    if(command == "open") {
        commandOpen(url);
    } else {
        qWarning() << "ShemeHandler::open unknow command:" << command;
    }
}

void ShemeHandler::commandOpen(const QUrl &url)
{
    QString type = url.path().remove('/').toLower();
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
    ml_return_on_fail2(url.hasQueryItem("id"), "MoltaqaShemeHandler::openBook url doesn't have an id query item");

    int bookID = url.queryItemValue("id").toInt();
    int page = url.hasQueryItem("page") ? url.queryItemValue("page").toInt() : -1;

    BooksViewer::instance()->openBook(bookID, page);
}

void ShemeHandler::openQuran(const QUrl &url)
{
    ml_return_on_fail2(url.hasQueryItem("sora"), "MoltaqaShemeHandler::openQuran url doesn't have a sora query item");

    int sora = url.queryItemValue("sora").toInt();
    int aya = url.hasQueryItem("aya") ? url.queryItemValue("aya").toInt() : 1;

    LibraryBookPtr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
    ml_return_on_fail2(quranBook, "MoltaqaShemeHandler::openQuran quranBook is null");

    BookWidget *w = BooksViewer::instance()->openBook(quranBook->id);
    ml_return_on_fail2(w, "MoltaqaShemeHandler::openQuran quranBook is null");

    w->openSora(sora, aya);
}

void ShemeHandler::openAuthor(const QUrl &url)
{
    ml_return_on_fail2(url.hasQueryItem("id"), "MoltaqaShemeHandler::openAuthor url doesn't have an id query item");
    int author = url.queryItemValue("id").toInt();

    AuthorsView::instance()->openAuthorInfo(author);
}

void ShemeHandler::openRawi(const QUrl &url)
{
    ml_return_on_fail2(url.hasQueryItem("id"), "MoltaqaShemeHandler::openRawi url doesn't have an id query item");
    int rawi = url.queryItemValue("id").toInt();

    TarajemRowatView::instance()->openRawiInfo(rawi);
}
