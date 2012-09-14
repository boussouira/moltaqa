#ifndef BOOKEXCEPTION_H
#define BOOKEXCEPTION_H

#include <qstring.h>

class QWidget;

class BookException
{
public:
    BookException();
    BookException(const QString &what, int id=0);
    BookException(const QString &what, const QString &file, int id=0);
    ~BookException();

    QString what() const;
    int id() const;
    void setWhat(const QString &what);
    void setId(int id);

    QString format(bool html=false);
    void print();
    void showMessage(QString title="", QWidget *parent=0);

protected:
    QString m_what;
    QString m_file;
    int m_id;
};

#endif // BOOKEXCEPTION_H
