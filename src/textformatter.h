#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qsettings.h>
#include <qstack.h>

class LibraryBook;
class BookPage;

class TextFormatter : public QObject
{
    Q_OBJECT

public:
    TextFormatter(QObject *parent = 0);

    void setData(LibraryBook *book, BookPage *page);

    QString getText();
    void insertText(QString text);

    QString getHtmlView(QString text=QString());

protected:
    void insertHtmlTag(QString tag, QString text, QString className="", QString idName="");
    void insertDivTag(QString text, QString className="", QString idName="");
    void insertSpanTag(QString text, QString className="", QString idName="");
    void insertImage(QString src);
    void openHtmlTag(QString tag, QString className="", QString idName="");
    void closeHtmlTag(QString tag=QString());
    void closeAllTags();
    void addCSS(QString cssFile);
    void addJS(QString jsFile);
    void addJSCode(QString jsCode);

protected:
    void laodSettings();
    void clearText();
    void genHeaderAndFooter();

public slots:
    void start();
    void done();

signals:
    void startReading();
    void doneReading();

protected:
    LibraryBook *m_book;
    BookPage *m_page;
    QString m_styleFile;
    QString m_jqueryFile;
    QString m_scriptFile;
    QString m_cssID;
    QString m_headerText;
    QString m_footerText;
    QString m_html;
    QStack<QString> m_openTags;
};

#endif // TEXTFORMATTER_H
