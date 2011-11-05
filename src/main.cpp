#include "mainwindow.h"
#include "utils.h"
#include <qapplication.h>
#include <qtranslator.h>
#include <qlocale.h>
#include <qtextcodec.h>
#include <qsettings.h>
#include <qdatetime.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setLayoutDirection(Qt::RightToLeft);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTranslator translator;
    translator.load(QString("qt_ar"), App::localeDir());
    app.installTranslator(&translator);

    app.setOrganizationName("Ahl-Alhdeeth");
    app.setOrganizationDomain("ahlalhdeeth.com");
    app.setApplicationName("Moltaqa-Library");
    app.setApplicationVersion("0.5");

    qsrand(QDateTime::currentDateTime().toTime_t());

    int ret = -1;

    MainWindow w;
    if(w.init()) {
        w.setLayoutDirection(Qt::RightToLeft);
        w.show();
        ret = app.exec();
    }

    return ret;
}
