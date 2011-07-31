#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QSettings>
#include "mainwindow.h"
#include <settingschecker.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTranslator translator;
    translator.load(QString("qt_ar"), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

    app.setOrganizationName("alkotobiya");
    app.setOrganizationDomain("alkotobiya.sf.net");
    app.setApplicationName("Al-Kotobiya");
    app.setApplicationVersion("0.5");

//    SettingsChecker checker(&app);
//    checker.checkSettings();

    MainWindow w;
    w.show();
    return app.exec();
}
