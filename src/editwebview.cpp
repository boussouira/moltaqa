#include "editwebview.h"
#include "bookeditorview.h"
#include "bookpage.h"
#include "htmlhelper.h"
#include "stringutils.h"
#include "utils.h"
#include "webpagenam.h"

#include <qsettings.h>

EditWebView::EditWebView(QWidget *parent) : WebView(parent)
{
    setupEditor();
}

void EditWebView::setupEditor(const QString &text)
{
    settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);

    settings()->setFontFamily(QWebSettings::StandardFont,
                              Utils::Settings::get("DefaultFont/fontFamily").toString());

    settings()->setFontSize(QWebSettings::DefaultFontSize,
                            Utils::Settings::get("DefaultFont/fontSize").toInt());

    QUrl style(QUrl::fromLocalFile(App::currentStyle("default.css")));

    HtmlHelper helper;

    helper.beginHtml();

    helper.beginHead();
    helper.setTitle("Editor");
    helper.addJS("jquery.js");
    helper.addJS("ckeditor/ckeditor.js");
    helper.addJS("scripts.js");
    helper.addJS("editor.js");
    helper.endHead();

    helper.beginBody();
    helper.beginHtmlTag("form", "", "method='post'");
    helper.insertHtmlTag("textarea", text, "#editor1", "cols='80' rows='10' name='editor1'");
    helper.addJSCode(QString("var editor; "
                "editor = CKEDITOR.replace('editor1', {"
                "contentsCss: '%3', "
                "on: {"
                "instanceReady : function()"
                "{ "
                "this.dataProcessor.writer.indentationChars = ''; "
                "this.execCommand('maximize'); }}});").arg(style.toString()));

    helper.endAll();

    setHtml(helper.html(), QUrl(WebPageNAM::baseUrl()));
}

void EditWebView::setEditorText(QString text)
{
    execJS(QString("setEditorText('%1')").arg(Utils::Html::jsEscape(text)));
    resetUndo();
}

QString EditWebView::editorText()
{
    return execJS("getEditorText()").toString();
}

bool EditWebView::pageModified()
{
    return execJS("editor.getCommand('undo').state==2").toBool();
}

void EditWebView::resetUndo()
{
    execJS("editor.resetUndo()");
}

void EditWebView::makeSelectTextTitle(QString text, int level, QString tagID)
{
    execJS(QString("makeSelectionTitle('%1', %2, '%3')")
           .arg(Utils::Html::jsEscape(text))
           .arg(level)
           .arg(Utils::Html::jsEscape(tagID)));
}

void EditWebView::insertImage(QString src)
{
    execJS(QString("inserImage('%1')").arg(Utils::Html::jsEscape(src)));
}
