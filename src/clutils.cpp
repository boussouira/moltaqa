#include "clutils.h"

QString Utils::WCharToString(const wchar_t *str)
{
    return QString::fromWCharArray(str);
}

wchar_t* Utils::QStringToWChar(const QString &str)
{
    wchar_t *string = new wchar_t[str.length()+1];
    str.toWCharArray(string);
    string[str.length()] = 0;

    return string;
}
