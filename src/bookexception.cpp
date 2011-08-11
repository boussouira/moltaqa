#include "bookexception.h"

BookException::BookException()
{
}

BookException::BookException(const QString &what, int id)
{
    m_id = id;
    m_what = what;
}

BookException::BookException(const QString &description, const QString &file, int id)
{
    m_id = id;
    m_what = QString("<p>%1<br /><strong>%2</strong></p>").arg(description).arg(file);
}

BookException::~BookException()
{
}

