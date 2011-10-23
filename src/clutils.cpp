#include "clutils.h"
#include <stdlib.h>

QString Utils::WCharToString(const wchar_t *str)
{
    return QString::fromWCharArray(str);
}

wchar_t* Utils::QStringToWChar(const QString &str)
{
    wchar_t *string = (wchar_t*) malloc((str.length()+1) * sizeof(wchar_t));
    str.toWCharArray(string);
    string[str.length()] = 0;

    return string;
}
