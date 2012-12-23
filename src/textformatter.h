#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qstack.h>

#include "htmlhelper.h"
#include "librarybook.h"

class TextFormatter : public QObject
{
    Q_OBJECT

public:
    TextFormatter(QObject *parent = 0);

    void setData(LibraryBook::Ptr book, BookPage *page);

    QString getText();
    void insertText(QString text);

    QString getHtmlView(const QString &text=QString(), const QString &jsCode=QString());

protected:
    void laodSettings();
    void clearText();

public slots:
    void start();
    void done();

signals:
    void startReading();
    void doneReading();

protected:
    LibraryBook::Ptr m_book;
    BookPage *m_page;
    QString m_cssID;
    QString m_headerText;
    QString m_footerText;
    QString m_html;
    HtmlHelper m_htmlHelper;
    QStack<QString> m_openTags;
};

#endif // TEXTFORMATTER_H
