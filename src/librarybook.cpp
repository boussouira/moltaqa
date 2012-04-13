#include "librarybook.h"
#include "utils.h"
#include <qfile.h>
#include <qstringlist.h>

LibraryBook::LibraryBook()
{
    id = 0;
    authorID = 0;
    bookFlags = 0;
    indexFlags = NotIndexed;
}

LibraryBook::~LibraryBook()
{
}

bool LibraryBook::exists()
{
    if(!path.isEmpty()){
        return QFile::exists(path);
    } else {
        qWarning("Call to BookInfo::exists() before BookInfo::setBookPath()");
        return false;
    }
}

LibraryBook *LibraryBook::clone()
{
    return new LibraryBook(*this);
}

QDebug operator<<(QDebug dbg, LibraryBookPtr &info)
{
    dbg.nospace() << "BookInfo(\n\t"
                  << "ID: " << info->id << "\n\t"
                  << "Type: " << info->type << "\n\t"
                  << "Book name: " << info->title << "\n\t"
                  << "Path: " << info->path << "\n\t"
                  << "Author: " << info->authorName << "\n\t"
                  << "Edition: " << info->edition << "\n\t"
                  << "Publisher: " << info->publisher << "\n\t"
                  << "Mohaqeq: " << info->mohaqeq<< "\n\t"
                  << "Info: " << info->info << "\n"
                  << ")";

    return dbg.space();
}
