#include "librarybook.h"
#include "utils.h"
#include "xmldomhelper.h"
#include <qfile.h>
#include <qstringlist.h>
#include <qdom.h>

LibraryBookMeta::LibraryBookMeta()
{
    id = 0;
    createDate = 0;
    importDate = 0;
    updateDate = 0;
    openCount = 0;
    updateCount = 0;
    resultOpenCount = 0;
}

QString LibraryBookMeta::createDateStr()
{
    return formatDate(createDate);
}

QString LibraryBookMeta::importDateStr()
{
    return formatDate(importDate);
}

QString LibraryBookMeta::updateDateStr()
{
    return formatDate(updateDate);
}

QString LibraryBookMeta::formatDate(uint timestamp)
{
    return QDateTime::fromTime_t(timestamp).toString("dd/MM/yyyy - hh:mm:ss");
}

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
    uuid = bookElement.attribute("uuid");
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
    bookElement.setAttribute("uuid", uuid);
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

QDebug operator<<(QDebug dbg, LibraryBook::Ptr &info)
{
    dbg.nospace() << "BookInfo(\n\t"
                  << "ID: " << info->id << "\n\t"
                  << "UUID: " << info->uuid << "\n\t"
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
