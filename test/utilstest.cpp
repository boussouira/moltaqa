#include "utilstest.h"
#include "utils.h"

#define u(x) QString::fromUtf8(x)

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

void UtilsTest::arClean()
{
    QCOMPARE(Utils::arClean(u("احمد")), u("احمد"));

    QCOMPARE(Utils::arClean(u("أحمد")), u("احمد"));

    QCOMPARE(Utils::arClean(u("إِيَّاكَ نَعْبُدُ وَإِيَّاكَ نَسْتَعِينُ")),
             u("اياك نعبد واياك نستعين"));

    QCOMPARE(Utils::arClean(u("إِنَّ فِي ذَلِكَ لَآيَةً لِقَوْمٍ يَتَفَكَّرُونَ")),
             u("ان في ذلك لايه لقوم يتفكرون"));
}

void UtilsTest::arCompare()
{
    QVERIFY(Utils::arCompare(u("وَتَرَى الْفُلْكَ مَوَاخِرَ فِيهِ وَلِتَبْتَغُوا مِنْ فَضْلِهِ وَلَعَلَّكُمْ تَشْكُرُونَ"),
                              u("وتري الفلك مواخر فيه ولتبتغوا من فضله ولعلكم تشكرون")));
    QVERIFY(Utils::arCompare(u("قُلْ هُوَ اللَّهُ أَحَدٌ"),
                             u("قل هُوَ الله احد")));
}

void UtilsTest::arContains()
{
    QVERIFY(Utils::arContains(u("الله اكبر"), u("الله")));
    QVERIFY(! Utils::arContains(u("الله"), u("الله اكبر")));
}

void UtilsTest::arFuzzyContains()
{
    QVERIFY(Utils::arFuzzyContains(u("الله اكبر"), u("الله")));
    QVERIFY(Utils::arFuzzyContains(u("الله"), u("الله اكبر")));
    QVERIFY(Utils::arFuzzyContains(u("الله"), u("قُلْ هُوَ اللَّهُ أَحَدٌ")));
}

QTEST_MAIN(UtilsTest)
