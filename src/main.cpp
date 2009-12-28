#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setLayoutDirection(Qt::RightToLeft);
    MainWindow w;
    w.selectSora(1);
    w.show();
    return app.exec();
}
