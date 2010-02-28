#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load(QString("qt_ar"), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

    app.setOrganizationName("alkotobiya");
    app.setOrganizationDomain("alkotobiya.sf.net");
    app.setApplicationName("Al-Kotobiya");
    app.setApplicationVersion("0.5");

    MainWindow w;
    w.selectSora(1);
    w.show();
    return app.exec();
}
