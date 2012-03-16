#include "editwebview.h"
#include "utils.h"
#include "bookpage.h"
#include "bookeditorview.h"
#include "htmlhelper.h"
#include <QDebug>

EditWebView::EditWebView(QWidget *parent) : WebView(parent)
{
    setupEditor();
}

void EditWebView::setupEditor(const QString &text)
{
    settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);

    QUrl ckeditor(QUrl::fromLocalFile(App::jsDir() + "/ckeditor/ckeditor.js"));
    QUrl style(QUrl::fromLocalFile(App::stylesDir() + "/default/default.css"));
    QUrl script(QUrl::fromLocalFile(App::jsDir() + "/scripts.js"));

    HtmlHelper helper;

    helper.beginHtml();

    helper.beginHead();
    helper.setTitle("Editor");
    helper.addJS(ckeditor.toString());
    helper.addJS(script.toString());
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
                "this.dataProcessor.writer.lineBreakChars = ''; "
                "this.dataProcessor.writer.indentationChars = ''; "
                "this.execCommand('maximize'); }}});").arg(style.toString()));

    helper.endAllTags();

    setHtml(helper.html());
}

void EditWebView::setEditorText(QString text)
{
    execJS(QString("setEditorText('%1')").arg(HtmlHelper::jsEscape(text)));
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
