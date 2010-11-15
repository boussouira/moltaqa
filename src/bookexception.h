#ifndef BOOKEXCEPTION_H
#define BOOKEXCEPTION_H

//#include <exception>
#include <qstring.h>

class BookException /*: public std::exception*/
{
public:
    BookException();
    BookException(const QString &what, int id=0);
    BookException(const QString &description, const QString &file, int id=0);
    ~BookException();

    QString what() const { return m_what;}
    int id() const { return m_id;}
    void setWhat(const QString &what) { m_what = what; }
    void setId(int id) { m_id = id; }

protected:
    QString m_what;
    int m_id;
};

#endif // BOOKEXCEPTION_H
