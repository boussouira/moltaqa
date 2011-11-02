#include "utilstest.h"
#include "utils.h"

UtilsTest::UtilsTest()
{
}

void UtilsTest::randInt()
{
    int count = 1000;
    int rmin = 0;
    int rmax = 100;
    bool randInt = true;

    while(--count>0) {
        int rnd = Utils::randInt(rmin, rmax);
        randInt = (rmin <= rnd && rnd <= rmax);

        if(!randInt)
            break;
    }

    QVERIFY(randInt);
}

void UtilsTest::generateFileName()
{
    // Defaut file name prefix
    QString name = Utils::genBookName(QApplication::applicationDirPath());
    QVERIFY(name.startsWith("book_"));
    QVERIFY(name.endsWith(".alb"));

    QString name3 = Utils::genBookName(QApplication::applicationDirPath(), true);
    QVERIFY(name3.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name3.endsWith(".alb"));

    // Custom suffix
    QString name2 = Utils::genBookName(QApplication::applicationDirPath(), false, "sqlite");
    QVERIFY(name2.startsWith("book_"));
    QVERIFY(name2.endsWith(".sqlite"));

    QString name4 = Utils::genBookName(QApplication::applicationDirPath(), true, "sqlite");
    QVERIFY(name4.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name4.endsWith(".sqlite"));

    // Custom file name prefix
    QString name5 = Utils::genBookName(QApplication::applicationDirPath(), false, "sqlite", "mdb_");
    QVERIFY(name5.startsWith("mdb_"));
    QVERIFY(name5.endsWith(".sqlite"));

    QString name6 = Utils::genBookName(QApplication::applicationDirPath(), true, "sqlite", "mdb_");
    QVERIFY(name6.startsWith(QString("%1/mdb_").arg(QApplication::applicationDirPath())));
    QVERIFY(name6.endsWith(".sqlite"));
}

void UtilsTest::appDirs()
{
    qDebug() << "\n\t"
            <<  "appDir:" << App::appDir() << "\n\t"
            <<  "binDir:" <<   App::binDir() << "\n\t"
            <<  "shareDir:" <<   App::shareDir() << "\n\t"
            <<  "stylesDir:" <<   App::stylesDir() << "\n\t"
            <<  "localeDir:" <<   App::localeDir();

    QVERIFY(QApplication::applicationDirPath().startsWith(App::appDir()));
    QVERIFY(App::binDir().endsWith("/bin"));
    QVERIFY(App::shareDir().endsWith("/share/moltaqa-lib"));
    QVERIFY(App::stylesDir().endsWith("/share/moltaqa-lib/styles"));
    QVERIFY(App::localeDir().endsWith("/share/moltaqa-lib/locale"));
}

QTEST_MAIN(UtilsTest)
