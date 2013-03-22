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

    void beginHtmlTag(const QString &tag, const QString &selector="", const QString &attr="");
    void endHtmlTag(const QString &tag="");

    void endAll();

    void insertLink(const QString &text, const QString &href, const QString &selector="");
    void insertImage(const QString &src, const QString &selector="", const QString &attr="");

    void addCSS(QString cssFile, bool fullPath=false);
    void addJS(QString jsFile, bool fullPath=false);
    void addJSCode(const QString &jsCode);

    void clear();
    inline QString html() { return m_html; }
    inline void appendText(QString text) { m_html.append(text); }

    inline void addDoctype() { m_html.append("<!DOCTYPE html>"); }
    inline void beginHtml() { beginHtmlTag("html"); }
    inline void beginBody() { beginHtmlTag("body"); }
    inline void beginHead() { beginHtmlTag("head"); }
    inline void beginStyle() { beginHtmlTag("style", "", "type='text/css'"); }

    inline void endHtml() { endHtmlTag(); }
    inline void endBody() { endHtmlTag(); }
    inline void endHead() { endHtmlTag(); }
    inline void endStyle() { endHtmlTag(); }

    HtmlHelper& beginStyleSelector(QString selector);
    HtmlHelper& addStyleRule(QString name, QString value);
    HtmlHelper& addStyleRules(QString styleRules);
    HtmlHelper& endStyleSelector();

    inline void setTitle(const QString &title) { insertHtmlTag("title", title); }

    inline void setCharset(const QString &charset="utf-8")
    {
        m_html.append(QString("<meta http-equiv=\"content-type\" content=\"text/html; charset=%1\" />").arg(charset));
    }

    inline void insertDiv(const QString &text, const QString &selector="")
    {
        insertHtmlTag("div", text, selector);
    }

    inline void insertSpan(const QString &text, const QString &selector="")
    {
        insertHtmlTag("span", text, selector);
    }

    inline void insertParagraph(const QString &text, const QString &selector="")
    {
        insertHtmlTag("p", text, selector);
    }

    inline void insertHead(int head, const QString &text, const QString &selector="")
    {
        insertHtmlTag(QString("h%1").arg(head), text, selector);
    }

    inline void beginDiv(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("div", selector, attr);
    }

    inline void beginParagraph(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("p", selector, attr);
    }

    inline void beginSpan(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("span", selector, attr);
    }

    /// Begin Definition List
    inline void beginDL(const QString &selector="", const QString &attr="")
    {
        beginHtmlTag("dl", selector, attr);
    }

    /// Insert Definition Item
    inline void insertDT(const QString &text, const QString &selector="")
    {
        insertHtmlTag("dt", text, selector);
    }

    /// Insert item description
    inline void insertDD(const QString &text, const QString &selector="")
    {
        insertHtmlTag("dd", text, selector);
    }

    inline void beginLink(const QString &href, const QString &selector="")
    {
        beginHtmlTag("a", selector, QString("href='%1'").arg(href));
    }

    inline void endDiv() { endHtmlTag(); }
    inline void endParagraph() { endHtmlTag(); }
    inline void endSpan() { endHtmlTag(); }
    inline void endDL() { endHtmlTag(); }
    inline void endLink() { endHtmlTag(); }

    inline void insertBr() { m_html.append("<br />"); }

    void insertAuthorLink(const QString &authorName, int authorID);

protected:
    void addSelector(QString selector);
    void addExtraAttr(QString attr);

protected:
    QString m_html;
    QDir m_jsDir;
    QStack<QString> m_openTags;
    QString m_styleSelector;
};

#endif // HTMLHELPER_H
