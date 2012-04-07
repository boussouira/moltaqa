#include "htmlhelper.h"
#include "utils.h"
#include <qstringlist.h>
#include <qurl.h>

HtmlHelper::HtmlHelper() :
    m_jsDir(App::jsDir())
{
}

void HtmlHelper::clear()
{
    m_html.clear();
    m_openTags.clear();
}

QString HtmlHelper::html()
{
    return m_html;
}

void HtmlHelper::append(QString text)
{
    m_html.append(text);
}

void HtmlHelper::addDoctype()
{
    m_html.append("<!DOCTYPE html>");
}

void HtmlHelper::beginHtml()
{
    beginHtmlTag("html");
}

void HtmlHelper::endHtml()
{
    endHtmlTag();
}

void HtmlHelper::beginBody()
{
    beginHtmlTag("body");
}

void HtmlHelper::endBody()
{
    endHtmlTag();
}

void HtmlHelper::beginHead()
{
    beginHtmlTag("head");
}

void HtmlHelper::endHead()
{
    endHtmlTag();
}

void HtmlHelper::setCharset(const QString &charset)
{
    m_html.append(QString("<meta http-equiv=\"content-type\" content=\"text/html; charset=%1\" />").arg(charset));
}

void HtmlHelper::setTitle(const QString &title)
{
    insertHtmlTag("title", title);
}

void HtmlHelper::insertHtmlTag(const QString &tag, const QString &text, const QString &selector, const QString &attr)
{
    m_html.append(QString("<%1").arg(tag));

    addSelector(selector);
    addExtraAttr(attr);

    m_html.append(QString(">%1</%2>").arg(text).arg(tag));
}

void HtmlHelper::insertDivTag(const QString &text, const QString &selector)
{
    insertHtmlTag("div", text, selector);
}

void HtmlHelper::insertSpanTag(const QString &text, const QString &selector)
{
    insertHtmlTag("span", text, selector);
}

void HtmlHelper::insertParagraphTag(const QString &text, const QString &selector)
{
    insertHtmlTag("p", text, selector);
}

void HtmlHelper::insertHeadTag(int head, const QString &text, const QString &selector)
{
    insertHtmlTag(QString("h%1").arg(head), text, selector);
}

void HtmlHelper::insertLinkTag(const QString &text, const QString &href, const QString &selector)
{
    beginHtmlTag("a", selector, QString("href='%1'").arg(href));
    m_html.append(text);
    endHtmlTag();
}

void HtmlHelper::insertImage(const QString &src)
{
    m_html.append("<img src=\"");
    m_html.append(src);
    m_html.append("\" />");
}

void HtmlHelper::beginHtmlTag(const QString &tag, const QString &selector, const QString &attr)
{
    m_html.append(QString("<%1").arg(tag));

    addSelector(selector);
    addExtraAttr(attr);

    m_html.append(">");

    m_openTags.push(tag);
}

void HtmlHelper::beginDivTag(const QString &selector, const QString &attr)
{
    beginHtmlTag("div", selector, attr);
}

void HtmlHelper::beginParagraphTag(const QString &selector, const QString &attr)
{
    beginHtmlTag("p", selector, attr);
}

void HtmlHelper::beginSpanTag(const QString &selector, const QString &attr)
{
    beginHtmlTag("span", selector, attr);
}

void HtmlHelper::endHtmlTag(const QString &tag)
{
    if(tag.isEmpty()) {
        if(!m_openTags.isEmpty())
            m_html.append(QString("</%1>").arg(m_openTags.pop()));
    } else {
        m_html.append(QString("</%1>").arg(tag));
    }
}

void HtmlHelper::endDivTag()
{
    endHtmlTag();
}

void HtmlHelper::endParagraphTag()
{
    endHtmlTag();
}

void HtmlHelper::endSpanTag()
{
    endHtmlTag();
}

void HtmlHelper::endAllTags()
{
    while(!m_openTags.isEmpty())
        m_html.append(QString("</%1>").arg(m_openTags.pop()));
}

void HtmlHelper::addCSS(QString cssFile)
{
    m_html.append(QString("<link href= \"%1\" rel=\"stylesheet\" type=\"text/css\" />").arg(cssFile));
}

void HtmlHelper::addJS(QString jsFile, bool fullPath)
{
    if(!fullPath)
        jsFile = QUrl::fromLocalFile(m_jsDir.filePath(jsFile)).toString();

    m_html.append(QString("<script type=\"text/javascript\" src=\"%1\"></script>").arg(jsFile));
}

void HtmlHelper::addJSCode(const QString &jsCode)
{
    m_html.append("<script type=\"text/javascript\">");
    m_html.append("//<![CDATA[\n");
    m_html.append(jsCode);
    m_html.append("\n//]]>");
    m_html.append("</script>");
}

QString HtmlHelper::jsEscape(QString text)
{
    return text.replace('\\', "\\\\")
            .replace('"', "\\\"")
            .replace('\'', "\\'")
            .replace('\t', "\\t")
            .replace('\n', "\\n")
            .replace('\r', "\\r");
}

void HtmlHelper::addSelector(QString selector)
{
    ML_ASSERT(!selector.isEmpty());

    if(!selector.contains('|')) {
        if(selector.startsWith('#'))
            m_html.append(QString(" id=\"%1\"").arg(selector.remove(0, 1)));
        else if(selector.startsWith('.'))
            m_html.append(QString(" class=\"%1\"").arg(selector.remove(0, 1)));
        else
            qDebug("Unknow selector: %s", qPrintable(selector));
    } else {
        foreach (QString sel, selector.split('|', QString::SkipEmptyParts)) {
            addSelector(sel);
        }
    }
}

void HtmlHelper::addExtraAttr(QString attr)
{
    if(!attr.isEmpty()) {
        m_html.append(' ');
        attr.replace('\'', '"');
        m_html.append(attr);
    }
}
