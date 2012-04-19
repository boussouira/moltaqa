#ifndef HTMLHELPER_H
#define HTMLHELPER_H

#include <qstring.h>
#include <qstack.h>
#include <qdir.h>

class HtmlHelper
{
public:
    HtmlHelper();

    void insertHtmlTag(const QString &tag, const QString &text, const QString &selector="", const QString &attr="");

    void insertLinkTag(const QString &text, const QString &href, const QString &selector="");
    void insertImage(const QString &src);

    void beginHtmlTag(const QString &tag, const QString &selector="", const QString &attr="");
    void endHtmlTag(const QString &tag="");

    void endAllTags();

    void addCSS(QString cssFile);
    void addJS(QString jsFile, bool fullPath=false);
    void addJSCode(const QString &jsCode);

    void clear();
    inline QString html() { return m_html; }
    inline void append(QString text) { m_html.append(text); }

    inline void addDoctype() { m_html.append("<!DOCTYPE html>"); }
    inline void beginHtml() { beginHtmlTag("html"); }
    inline void beginBody() { beginHtmlTag("body"); }
    inline void beginHead() { beginHtmlTag("head"); }
    inline void endHtml() { endHtmlTag(); }
    inline void endBody() { endHtmlTag(); }
    inline void endHead() { endHtmlTag(); }

    inline void setTitle(const QString &title) { insertHtmlTag("title", title); }

    inline void setCharset(const QString &charset="utf-8")
    {
        m_html.append(QString("<meta http-equiv=\"content-type\" content=\"text/html; charset=%1\" />").arg(charset));
    }

    inline void insertDivTag(const QString &text, const QString &selector="")
    {
        insertHtmlTag("div", text, selector);
    }

    inline void insertSpanTag(const QString &text, const QString &selector="")
    {
        insertHtmlTag("span", text, selector);
    }

    inline void insertParagraphTag(const QString &text, const QString &selector="")
    {
        insertHtmlTag("p", text, selector);
    }

    inline void insertHeadTag(int head, const QString &text, const QString &selector="")
    {
        insertHtmlTag(QString("h%1").arg(head), text, selector);
    }

    inline void beginDivTag(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("div", selector, attr);
    }

    inline void beginParagraphTag(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("p", selector, attr);
    }

    inline void beginSpanTag(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("span", selector, attr);
    }

    inline void endDivTag() { endHtmlTag(); }
    inline void endParagraphTag() { endHtmlTag(); }
    inline void endSpanTag() { endHtmlTag(); }

     inline void insertBr() { m_html.append("<br />"); }

    void insertAuthorLink(const QString &authorName, int authorID);

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
