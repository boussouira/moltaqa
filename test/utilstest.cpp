#include "utilstest.h"
#include "utils.h"
#include "stringutils.h"
#include "sqlutils.h"
#include <qsqldatabase.h>
#include <QSqlQuery>

#define u(x) QString::fromUtf8(x)

UtilsTest::UtilsTest()
{
}

void UtilsTest::randInt()
{
    int rmin = 0;
    int rmax = 100;

    Utils::Rand::srand();

    for(int i=0;i<1000;i++) {
        int rnd = Utils::Rand::number(rmin, rmax);

        QVERIFY(rmin <= rnd && rnd <= rmax);
    }

    int randSize = 32;
    QString lastStr;
    for(int i=0; i<10; i++) {
        QString str = Utils::Rand::string(randSize);

        QCOMPARE(str.size(), randSize);

        if(lastStr.size())
            QVERIFY(lastStr != str);

        lastStr = str;
    }
}

void UtilsTest::generateFileName()
{
    // Defaut file name prefix
    QString name = Utils::Rand::fileName(QApplication::applicationDirPath());
    QVERIFY(name.startsWith("book_"));
    QVERIFY(name.endsWith(".alb"));

    QString name3 = Utils::Rand::fileName(QApplication::applicationDirPath(), true);
    QVERIFY(name3.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name3.endsWith(".alb"));

    // Custom suffix
    QString name2 = Utils::Rand::fileName(QApplication::applicationDirPath(), false, "sqlite");
    QVERIFY(name2.startsWith("book_"));
    QVERIFY(name2.endsWith(".sqlite"));

    QString name4 = Utils::Rand::fileName(QApplication::applicationDirPath(), true, "sqlite");
    QVERIFY(name4.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name4.endsWith(".sqlite"));

    // Custom file name prefix
    QString name5 = Utils::Rand::fileName(QApplication::applicationDirPath(), false, "sqlite", "mdb_");
    QVERIFY(name5.startsWith("mdb_"));
    QVERIFY(name5.endsWith(".sqlite"));

    QString name6 = Utils::Rand::fileName(QApplication::applicationDirPath(), true, "sqlite", "mdb_");
    QVERIFY(name6.startsWith(QString("%1/mdb_").arg(QApplication::applicationDirPath())));
    QVERIFY(name6.endsWith(".sqlite"));
}

void UtilsTest::arClean()
{
    QCOMPARE(Utils::String::Arabic::clean(u("احمد")), u("احمد"));

    QCOMPARE(Utils::String::Arabic::clean(u("أحمد  ")), u("احمد"));

    QCOMPARE(Utils::String::Arabic::clean(u("إِيَّاكَ نَعْبُدُ وَإِيَّاكَ نَسْتَعِينُ")),
             u("اياك نعبد واياك نستعين"));

    QCOMPARE(Utils::String::Arabic::clean(u("إِنَّ فِي ذَلِكَ لَآيَةً لِقَوْمٍ يَتَفَكَّرُونَ")),
             u("ان في ذلك لايه لقوم يتفكرون"));
}

void UtilsTest::arCompare()
{
    QVERIFY(Utils::String::Arabic::compare(u("وَتَرَى الْفُلْكَ مَوَاخِرَ فِيهِ وَلِتَبْتَغُوا مِنْ فَضْلِهِ وَلَعَلَّكُمْ تَشْكُرُونَ"),
                              u("وتري الفلك مواخر فيه ولتبتغوا من فضله ولعلكم تشكرون")));
    QVERIFY(Utils::String::Arabic::compare(u("قُلْ هُوَ اللَّهُ أَحَدٌ"),
                             u("قل هُوَ الله احد")));
}

void UtilsTest::arContains()
{
    QVERIFY(Utils::String::Arabic::contains(u("الله اكبر"), u("الله")));
    QVERIFY(! Utils::String::Arabic::contains(u("الله"), u("الله اكبر")));
}

void UtilsTest::arFuzzyContains()
{
    QVERIFY(Utils::String::Arabic::fuzzyContains(u("الله اكبر"), u("الله")));
    QVERIFY(Utils::String::Arabic::fuzzyContains(u("الله"), u("الله اكبر")));
    QVERIFY(Utils::String::Arabic::fuzzyContains(u("الله"), u("قُلْ هُوَ اللَّهُ أَحَدٌ")));
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
        QCOMPARE(excpected[i], Utils::Html::removeSpecialChars(origins[i]));
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
        QCOMPARE(excpected[i], Utils::Html::specialCharsEncode(origins[i]));
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
        QCOMPARE(excpected[i], Utils::Html::removeTags(origins[i]));
}

void UtilsTest::getTags()
{
    QStringList origins;
    QStringList excpected;

    origins << "Sime test<sanad>This is a test</sanad>, test an other tag:"
              "<sanad>I mean <span>THIS</span> tag</sanad> if it work.";

    excpected << "This is a test I mean  THIS  tag ";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::Html::getTagsText(origins[i], "sanad"));
}

void UtilsTest::getPageTitle()
{
    QList<int> titles;
    QList<int> pages;
    QList<int> excpectedTitles;

    titles << 3 << 7 << 8 << 20 << 50 << 100;

    pages           << 1 << 2 << 3 << 4 << 5 << 7 << 75 << 100 << 140;
    excpectedTitles << 3 << 3 << 3 << 3 << 3 << 7 << 50 << 100 << 100;

    for(int i=0; i<pages.size(); i++)
        QCOMPARE(getPageTitleID(titles, pages[i]), excpectedTitles[i]);

}

void UtilsTest::formatHTML()
{
    QStringList origins;
    QStringList excpected;

    origins << "The head\n\nSmall paragraph\nAn other one\n\nSecondHead"
            << "Line one\nLine Two\nLine Tree"
            << "\n\nLine one\nLine Two\nLine Tree\n\n\n";

    excpected << "<p>The head</p><p>Small paragraph<br />An other one</p><p>SecondHead</p>"
              << "<p>Line one<br />Line Two<br />Line Tree</p>"
              << "<p>Line one<br />Line Two<br />Line Tree</p>";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::Html::format(origins[i]));
}

void UtilsTest::queryBuilder()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", "TestQueryBuilder");
    db.setDatabaseName(QDir::current().absoluteFilePath("test.db"));

    QVERIFY(db.open());

    QSqlQuery query(db);

    int id = Utils::Rand::number(1, 500);

    // Create new table
    QueryBuilder q;
    q.setTableName("BooksInfo");
    q.setIgnoreExistingTable(true);
    q.setQueryType(QueryBuilder::Create);

    q.set("id", "INT");
    q.set("name", "TEXT");
    q.set("path", "TEXT");

    QVERIFY(q.exec(query));

    // Insert values to table
    for(int i=0; i<5; i++) {
        q.setQueryType(QueryBuilder::Insert);

        q.set("id", id+i);
        q.set("name", "The rate of hacking");
        q.set("path", "test.txt");

        QVERIFY(q.exec(query));
    }

    // Update values
    q.setQueryType(QueryBuilder::Update);

    q.set("name", "The Art Of Hacking");
    q.set("path", "test.pdf");

    q.where("id", id);
    q.where("path", "test.txt");

    QVERIFY(q.exec(query));

    // Select updated value
    q.setQueryType(QueryBuilder::Select);
    q.select("name");
    q.select("path");

    q.orderBy("id", QueryBuilder::Asc);

    QVERIFY(q.exec(query));
    QVERIFY(query.next());

    QCOMPARE(query.value(0).toString(), QString("The Art Of Hacking"));
    QCOMPARE(query.value(1).toString(), QString("test.pdf"));

    // Select updated value with where
    q.setQueryType(QueryBuilder::Select);
    q.select("name");
    q.select("path");

    q.where("id", id);

    q.orderBy("id", QueryBuilder::Desc);
    q.limit(1);

    QVERIFY(q.exec(query));
    QVERIFY(query.next());

    QCOMPARE(query.value(0).toString(), QString("The Art Of Hacking"));
    QCOMPARE(query.value(1).toString(), QString("test.pdf"));
}

void UtilsTest::cleanupTestCase()
{
    QSqlDatabase::removeDatabase("TestQueryBuilder");

    QVERIFY(QFile::remove(QDir::current().absoluteFilePath("test.db")));
}

int UtilsTest::getPageTitleID(QList<int> &titles, int pageID)
{
    if(!titles.contains(pageID)) {
        int title = 0;
        for(int i=0; i<titles.size(); i++) {
            title = titles.at(i);
            if(i == titles.size()-1)
                return titles.last();
            else if(title <= pageID && pageID < titles.at(i+1))
                return title;
            if(title > pageID)
                break;
        }

        return titles.first();
    }

    return pageID;
}

QTEST_MAIN(UtilsTest)
