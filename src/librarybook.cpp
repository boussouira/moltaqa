#include "librarybook.h"
#include "utils.h"
#include "xmldomhelper.h"
#include <qfile.h>
#include <qstringlist.h>
#include <qdom.h>

LibraryBook::LibraryBook()
{
    id = 0;
    authorID = 0;
    bookFlags = NoBookFlags;
    indexFlags = NotIndexed;
}

LibraryBook::~LibraryBook()
{
}

bool LibraryBook::exists()
{
    ml_return_val_on_fail2(path.size(), "LibraryBook::exists path is empty", false);
    ml_return_val_on_fail2(QFile::exists(path), "LibraryBook::exists book" << path << "doesn't exists", false);

    return true;
}

LibraryBook *LibraryBook::clone()
{
    return new LibraryBook(*this);
}

void LibraryBook::fromDomElement(QDomElement &bookElement)
{
    id = bookElement.attribute("id").toInt();
    authorID = bookElement.attribute("author").toInt();
    type = static_cast<LibraryBook::Type>(bookElement.attribute("type").toInt());
    bookFlags = static_cast<LibraryBook::BookFlags>(bookElement.attribute("flags").toInt());
    fileName = bookElement.firstChildElement("fileName").text();
    title = bookElement.firstChildElement("title").text();
    otherTitles = bookElement.firstChildElement("otherTitles").text();
    edition = bookElement.firstChildElement("edition").text();
    publisher = bookElement.firstChildElement("publisher").text();
    mohaqeq = bookElement.firstChildElement("mohaqeq").text();
    comment = bookElement.firstChildElement("comment").text();
    info = bookElement.firstChildElement("info").text();
}

void LibraryBook::toDomElement(XmlDomHelper &domHeleper, QDomElement &bookElement)
{
    bookElement.setAttribute("id", id);
    bookElement.setAttribute("type", type);
    bookElement.setAttribute("author", authorID);
    bookElement.setAttribute("flags", bookFlags);

    domHeleper.setElementText(bookElement, "fileName", fileName);

    domHeleper.setElementText(bookElement, "title", title);

    if(otherTitles.size())
        domHeleper.setElementText(bookElement, "otherTitles", otherTitles);

    if(edition.size())
        domHeleper.setElementText(bookElement, "edition", edition);

    if(publisher.size())
        domHeleper.setElementText(bookElement, "publisher", publisher);

    if(mohaqeq.size())
        domHeleper.setElementText(bookElement, "mohaqeq", mohaqeq);

    if(comment.size())
        domHeleper.setElementText(bookElement, "comment", comment, true);

    if(info.size())
        domHeleper.setElementText(bookElement, "info", info, true);
}

QDebug operator<<(QDebug dbg, LibraryBookPtr &info)
{
    dbg.nospace() << "BookInfo(\n\t"
                  << "ID: " << info->id << "\n\t"
                  << "Type: " << info->type << "\n\t"
                  << "Book name: " << info->title << "\n\t"
                  << "File name: " << info->fileName << "\n\t"
                  << "Path: " << info->path << "\n\t"
                  << "Author: " << info->authorName << "\n\t"
                  << "Edition: " << info->edition << "\n\t"
                  << "Publisher: " << info->publisher << "\n\t"
                  << "Mohaqeq: " << info->mohaqeq<< "\n\t"
                  << "comment: " << info->comment<< "\n\t"
                  << "Info: " << info->info << "\n"
                  << ")";

    return dbg.space();
}
