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
    ML_ASSERT_RET2(!path.isEmpty(), "LibraryBook::exists path is empty", false);
    ML_ASSERT_RET2(QFile::exists(path), "LibraryBook::exists book" << path << "doesn't exists", false);

    return true;
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
                  << "comment: " << info->comment<< "\n\t"
                  << "Info: " << info->info << "\n"
                  << ")";

    return dbg.space();
}
