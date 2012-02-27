#ifndef HTMLHELPER_H
#define HTMLHELPER_H

#include <qstring.h>
#include <qstack.h>

class HtmlHelper
{
public:
    HtmlHelper();

    void clear();
    QString html();

    void append(QString text);

    void addDoctype();

    void beginHtml();
    void endHtml();

    void beginBody();
    void endBody();

    void beginHead();
    void endHead();

    void setCharset(QString charset="utf-8");
    void setTitle(QString title);

    void insertHtmlTag(QString tag, QString text, QString selector="", QString attr="");
    void insertDivTag(QString text, QString selector="");
    void insertSpanTag(QString text, QString selector="");
    void insertParagraphTag(QString text, QString selector="");
    void insertImage(QString src);

    void beginHtmlTag(QString tag, QString selector="", QString attr="");
    void beginDivTag(QString selector="", QString attr="");
    void beginSpanTag(QString selector="", QString attr="");
    void endHtmlTag(QString tag="");
    void endDivTag();
    void endSpanTag();

    void endAllTags();

    void addCSS(QString cssFile);
    void addJS(QString jsFile);
    void addJSCode(QString jsCode);

protected:
    void addSelector(QString selector);
    void addExtraAttr(QString attr);

protected:
    QString m_html;
    QStack<QString> m_openTags;
};

#endif // HTMLHELPER_H
