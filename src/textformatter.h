#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <qobject.h>
#include <qstring.h>
#include <qregexp.h>
#include <qsettings.h>

class TextFormatter
{
public:
    TextFormatter();
    void laodSettings();
    QString getText();
    void insertText(QString text);
    void clearText();
    void insertHtmlTag(QString tag, QString text, QString className="", QString idName="");
    void insertDivTag(QString text, QString className="", QString idName="");
    void insertSpanTag(QString text, QString className="", QString idName="");


protected:
    QString m_styleFile;
    QString m_text;
    QString m_cssID;
};

#endif // TEXTFORMATTER_H
