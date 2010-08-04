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

protected:
    QString m_styleFile;
};

#endif // TEXTFORMATTER_H
