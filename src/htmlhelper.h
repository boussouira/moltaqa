#ifndef HTMLHELPER_H
#define HTMLHELPER_H

#include <qstring.h>
#include <qstack.h>
#include <qdir.h>

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

    void setCharset(const QString &charset="utf-8");
    void setTitle(const QString &title);

    void insertHtmlTag(const QString &tag, const QString &text, const QString &selector="", const QString &attr="");
    void insertDivTag(const QString &text, const QString &selector="");
    void insertSpanTag(const QString &text, const QString &selector="");
    void insertParagraphTag(const QString &text, const QString &selector="");
    void insertHeadTag(int head, const QString &text, const QString &selector="");
    void insertLinkTag(const QString &text, const QString &href, const QString &selector="");
    void insertImage(const QString &src);

    void beginHtmlTag(const QString &tag, const QString &selector="", const QString &attr="");
    void beginDivTag(const QString &selector="", const QString &attr="");
    void beginParagraphTag(const QString &selector="", const QString &attr="");
    void beginSpanTag(const QString &selector="", const QString &attr="");
    void endHtmlTag(const QString &tag="");
    void endDivTag();
    void endParagraphTag();
    void endSpanTag();

    void endAllTags();

    void addCSS(QString cssFile);
    void addJS(QString jsFile, bool fullPath=false);
    void addJSCode(const QString &jsCode);

    static QString jsEscape(QString text);

protected:
    void addSelector(QString selector);
    void addExtraAttr(QString attr);

protected:
    QString m_html;
    QDir m_jsDir;
    QStack<QString> m_openTags;
};

#endif // HTMLHELPER_H
