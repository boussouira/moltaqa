#include "mainwindow.h"
#include "utils.h"
#include <qapplication.h>
#include <qtranslator.h>
#include <qlocale.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qdatetime.h>
#include <qdebug.h>

#ifdef Q_OS_WIN
#include <Windows.h>

void setArabicKeyboardLayout()
{
    HKL local = LoadKeyboardLayout(L"00000401", KLF_ACTIVATE);

    if(local != NULL)
        ActivateKeyboardLayout(local, KLF_ACTIVATE);
    else
        qWarning("Can't load Arabic Keyboard Layout");
}

#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setLayoutDirection(Qt::RightToLeft);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

#ifdef Q_OS_WIN
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
#endif

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTranslator translator;
    translator.load(QString("qt_ar"), App::localeDir());
    app.installTranslator(&translator);

    app.setOrganizationName("Ahl-Alhdeeth");
    app.setOrganizationDomain("ahlalhdeeth.com");
    app.setApplicationName("Moltaqa-Library");
    app.setApplicationVersion("0.5");

    srand(uint(QDateTime::currentDateTime().toMSecsSinceEpoch() & 0xFFFFFF));

#ifdef Q_OS_WIN
    setArabicKeyboardLayout();
#endif

#ifdef Q_OS_LINUX
    if(!setenv("MDB_JET3_CHARSET", "cp1256", 1))
        qCritical("main: setenv error");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1256"));
#endif

    int ret = -1;

    MainWindow w;
    if(w.init()) {
        w.setLayoutDirection(Qt::RightToLeft);
        w.show();
        ret = app.exec();
    }

    return ret;
}
