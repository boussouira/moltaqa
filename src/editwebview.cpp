#include "editwebview.h"
#include "utils.h"
#include "bookpage.h"
#include "bookeditorview.h"
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

    QString html(QString(
"<html> \
    <head> \
        <title>Editor</title> \
        <script type=\"text/javascript\" src=\"%1\"></script>\
        <script type=\"text/javascript\" src=\"%2\"></script>\
    </head>  \
<body>  \
        <form method=\"post\"> \
        <textarea cols=\"80\" id=\"editor1\" name=\"editor1\" rows=\"10\">%4</textarea>  \
        <script type=\"text/javascript\">  \
            var editor; \
            editor = CKEDITOR.replace('editor1',  \
            { \
                contentsCss: '%3', \
                on: \
                { \
                    instanceReady : function() \
                    { \
                        editor.execCommand('maximize');  \
                    } \
                } \
            }); \
        </script>  \
    </form> \
</body>  \
</html>")
.arg(ckeditor.toString())
.arg(script.toString())
.arg(style.toString())
.arg(text));

setHtml(html);
}

void EditWebView::setEditorText(QString text)
{
    execJS(QString("setEditorText('%1')").arg(text
                                              .replace('\'', "\\\'")
                                              .replace('\\', "\\\\")
                                              .replace(QRegExp("[\n\r]+"), "'+'")));
}

QString EditWebView::editorText()
{
    return execJS("getEditorText()").toString().remove(QRegExp("[\n\r]+"));
}
