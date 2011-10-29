#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qsettings.h>
#include <qstack.h>

class TextFormatter : public QObject
{
    Q_OBJECT

public:
    TextFormatter(QObject *parent = 0);

    QString getText();
    void insertText(QString text);
    void insertHtmlTag(QString tag, QString text, QString className="", QString idName="");
    void insertDivTag(QString text, QString className="", QString idName="");
    void insertSpanTag(QString text, QString className="", QString idName="");
    void openHtmlTag(QString tag, QString className="", QString idName="");
    void closeHtmlTag(QString tag=QString());

protected:
    void laodSettings();
    void clearText();

public slots:
    void start(bool clear=true);
    void done();

signals:
    void startReading();
    void doneReading();

protected:
    QString m_styleFile;
    QString m_jqueryFile;
    QString m_scriptFile;
    QString m_cssID;
    QString m_text;
    QString m_html;
    QStack<QString> m_openTags;
};

#endif // TEXTFORMATTER_H
