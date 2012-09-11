#include "bookexception.h"
#include <qdebug.h>
#include <qmessagebox.h>

BookException::BookException()
{
    m_id = 0;
}

BookException::BookException(const QString &what, int id)
{
    m_id = id;
    m_what = what;
}

BookException::BookException(const QString &what, const QString &file, int id)
{
    m_id = id;
    m_what = what;
    m_file = file;
}

BookException::~BookException()
{
}

QString BookException::what() const
{
    return m_what;
}

int BookException::id() const
{
    return m_id;
}

void BookException::setWhat(const QString &what)
{
    m_what = what;
}

void BookException::setId(int id)
{
    m_id = id;
}

QString BookException::format(bool html)
{
    QString str;

    if(html)
        str = "<strong>" + m_what + "</strong>";
    else
        str = m_what;

    if(m_id != 0) {
        if(html)
            str += "<br />";
        else
            str += " - ";

        str += QString("Error code: %1").arg(m_id);
    }

    if(m_file.size()) {
        if(html)
            str += "<br />";
        else
            str += " - ";

        str += QString("File: %1").arg(m_file);
    }

    return str;
}

void BookException::print()
{
    qCritical() << "Exception:" << format();
}

void BookException::showMessage(QString title, QWidget *parent)
{
    if(title.isEmpty())
        title = QWidget::tr("خطأ");

    QMessageBox::critical(parent, title, format(true));
}
