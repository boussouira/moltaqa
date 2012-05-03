#include "mainwindow.h"
#include "utils.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qtsingleapplication.h>
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
    QtSingleApplication app(argc, argv);

    QString message;
    for (int a = 1; a < argc; ++a) {
        message += argv[a];
        if (a < argc-1)
            message += " ";
    }
    if (app.sendMessage(message))
        return 0;

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

    Utils::Rand::srand();

#ifdef Q_OS_WIN
    setArabicKeyboardLayout();
#endif

#ifdef USE_MDBTOOLS
    if(setenv("MDB_JET3_CHARSET", "cp1256", 1))
        qCritical("main: setenv error");

    MdbConverter::init();
#endif

    int ret = -1;

    MainWindow w;
    if(w.init()) {
        w.setLayoutDirection(Qt::RightToLeft);
        w.show();

        QObject::connect(&app, SIGNAL(messageReceived(QString)),
                         &w, SLOT(handleMessage(QString)));

        app.setActivationWindow(&w);

        ret = app.exec();
    }

#ifdef USE_MDBTOOLS
    MdbConverter::exit();
#endif

    return ret;
}
