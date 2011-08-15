#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qsettings.h>

class TextFormatter : public QObject
{
    Q_OBJECT

public:
    TextFormatter(QObject *parent = 0);
    void laodSettings();
    QString getText();
    void insertText(QString text);
    void insertHtmlTag(QString tag, QString text, QString className="", QString idName="");
    void insertDivTag(QString text, QString className="", QString idName="");
    void insertSpanTag(QString text, QString className="", QString idName="");

protected:
    void clearText();

public slots:
    void start(bool clear=true);
    void done();

signals:
    void startReading();
    void doneReading(const QString &text);

protected:
    QString m_styleFile;
    QString m_cssID;
    QString m_text;
    QString m_html;
};

#endif // TEXTFORMATTER_H
