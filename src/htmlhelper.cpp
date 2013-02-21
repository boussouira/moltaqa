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

void HtmlHelper::insertHtmlTag(const QString &tag, const QString &text, const QString &selector, const QString &attr)
{
    m_html.append(QString("<%1").arg(tag));

    addSelector(selector);
    addExtraAttr(attr);

    m_html.append(QString(">%1</%2>").arg(text).arg(tag));
}

void HtmlHelper::insertLink(const QString &text, const QString &href, const QString &selector)
{
    beginHtmlTag("a", selector, QString("href='%1'").arg(href));
    m_html.append(text);
    endHtmlTag();
}

void HtmlHelper::insertImage(const QString &src, const QString &selector, const QString &attr)
{
    m_html.append("<img src=\"");
    m_html.append(src);
    m_html.append("\"");

    addSelector(selector);
    addExtraAttr(attr);

    m_html.append(" />");
}

void HtmlHelper::beginHtmlTag(const QString &tag, const QString &selector, const QString &attr)
{
    m_html.append(QString("<%1").arg(tag));

    addSelector(selector);
    addExtraAttr(attr);

    m_html.append(">");

    m_openTags.push(tag);
}

void HtmlHelper::endHtmlTag(const QString &tag)
{
    if(tag.isEmpty()) {
        if(m_openTags.size())
            m_html.append(QString("</%1>").arg(m_openTags.pop()));
    } else {
        m_html.append(QString("</%1>").arg(tag));
    }
}

void HtmlHelper::endAll()
{
    while(m_openTags.size())
        m_html.append(QString("</%1>").arg(m_openTags.pop()));
}

void HtmlHelper::addCSS(QString cssFile, bool fullPath)
{
    if(!fullPath)
        cssFile = QUrl::fromLocalFile(App::currentStyle(cssFile)).toString();

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

/**
 * @brief Add CSS attribute to current html tag.
 * Each attribute should start with (.) or (#) to specify a class or id
 * attribute, (.) for classes and (#) for ids.
 * To combine CSS selector use (|) or a space.
 * if you use (|) it will create a new html attribute for each CSS selector.
 * if you use a space it will combine class selector in a one class attribute.
 * example:
 *  "#search|.next|.current" => id="search" class="next" class="current"
 *  "#search .next .current" => id="search" class="next current"
 *
 *@param selector CSS selector
 */

void HtmlHelper::addSelector(QString selector)
{
    ml_return_on_fail(selector.size());

    if(selector.contains('|')) {
        foreach (QString sel, selector.split('|', QString::SkipEmptyParts)) {

            if(sel.startsWith('#'))
                m_html.append(QString(" id=\"%1\"").arg(sel.remove(0, 1)));
            else if(sel.startsWith('.'))
                m_html.append(QString(" class=\"%1\"").arg(sel.remove(0, 1)));
            else
                qDebug("HtmlHelper::addSelector Unknow selector: %s", qPrintable(sel));
        }
    } else {
        QString idAttr;
        QString classAttr;
        foreach (QString sel, selector.split(' ', QString::SkipEmptyParts)) {
            if(selector.startsWith('#'))
                idAttr.append(sel.remove(0, 1) + ' ');
            else if(selector.startsWith('.'))
                classAttr.append(sel.remove(0, 1) + ' ');
            else
                qDebug("HtmlHelper::addSelector Unknow selector: %s", qPrintable(sel));
        }

        if(idAttr.size())
            m_html.append(QString(" id=\"%1\"").arg(idAttr.trimmed()));

        if(classAttr.size())
            m_html.append(QString(" class=\"%1\"").arg(classAttr.trimmed()));
    }
}

void HtmlHelper::addExtraAttr(QString attr)
{
    if(attr.size()) {
        m_html.append(' ');
        attr.replace('\'', '"');
        m_html.append(attr);
    }
}

void HtmlHelper::insertAuthorLink(const QString &authorName, int authorID)
{
    insertLink(authorName, QString("moltaqa://?c=open&t=author&id=%1").arg(authorID));
}
