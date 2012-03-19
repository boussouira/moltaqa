#include "utilstest.h"
#include "utils.h"
#include "stringutils.h"

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

    QCOMPARE(Utils::arClean(u("أحمد  ")), u("احمد"));

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
                             u("قل هُوَ الله     احد")));
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

void UtilsTest::removeHtmlSpecialChars()
{
    QStringList origins;
    QStringList excpected;

    origins << "<p>Je suis &quot;ahmed&quot; j&#39;ai 15 &lt; age &gt; 18</p>"
            << "&lt;a href=&#039;test&#039;&gt;Test&lt;/a&gt;";

    excpected << "<p>Je suis  ahmed  j ai 15   age   18</p>"
              << " a href= test  Test /a ";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::removeHtmlSpecialChars(origins[i]));
}

void UtilsTest::htmlSpecialCharsEncode()
{
    QStringList origins;
    QStringList excpected;

    origins << "<p>Je suis \"ahmed\" j'ai 15 < age > 18</p>"
            << "<a href='test'>Test</a>";

    excpected << "&lt;p&gt;Je suis &quot;ahmed&quot; j&#39;ai 15 &lt; age &gt; 18&lt;/p&gt;"
              << "&lt;a href=&#39;test&#39;&gt;Test&lt;/a&gt;";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::htmlSpecialCharsEncode(origins[i]));
}

void UtilsTest::removeHtmlTags()
{
    QStringList origins;
    QStringList excpected;

    origins << "<p>Je suis \"kamal\" j'ai 15 < age > 18</p>"
            << "<a href='test'>Test</a>"
            << "<p>My name is <span style=\"color:red;\">kamal</span>from<img src\"pay.png\" /></p>"
            << "<ul dir=\"ltr\"><li><sanad>saad</sanad></li><li>kamal</li><li>karim</li><li><mateen>ahmed</mateen></li></ul>";

    excpected << "Je suis \"kamal\" j'ai 15 < age > 18"
              << "Test"
              << "My name is  kamal from"
              << "saad   kamal  karim   ahmed";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::removeHtmlTags(origins[i]));
}

void UtilsTest::getTags()
{
    QStringList origins;
    QStringList excpected;

    origins << "Sime test<sanad>This is a test</sanad>, test an other tag:"
              "<sanad>I mean <span>THIS</span> tag</sanad> if it work.";

    excpected << "This is a test I mean  THIS  tag ";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::getTagsText(origins[i], "sanad"));
}

QTEST_MAIN(UtilsTest)
