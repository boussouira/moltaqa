#ifndef EDITWEBVIEW_H
#define EDITWEBVIEW_H

#include "webview.h"

class BookPage;

class EditWebView : public WebView
{
    Q_OBJECT
public:
    EditWebView(QWidget* parent = 0);

    void setupEditor(const QString &text=QString());

    void setEditorText(QString text);
    QString editorText();

    bool pageModified();

    void resetUndo();

    void makeSelectTextTitle(QString text, int level, QString tagID);

public slots:
    void insertImage(QString src);

};

#endif // EDITWEBVIEW_H
