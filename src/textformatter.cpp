#include "textformatter.h"

TextFormatter::TextFormatter()
{
    laodSettings();
}

void TextFormatter::laodSettings()
{
    QSettings settings;
    QString style = settings.value("General/style", "Default").toString();
    m_styleFile = settings.value(QString("%1_style/style_file_path").arg(style)).toString();
#ifdef Q_OS_WIN32
    m_styleFile.prepend("file:///");
#else
    m_styleFile.prepend("file://");
#endif
}
