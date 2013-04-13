#include "utilstest.h"
#include "utils.h"
#include "stringutils.h"
#include "sqlutils.h"
#include "bookutils.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>

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
    QVERIFY(name.endsWith(".mlb"));

    QString name3 = Utils::Rand::fileName(QApplication::applicationDirPath(), true);
    QVERIFY(name3.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name3.endsWith(".mlb"));

    // Custom suffix
    QString name2 = Utils::Rand::fileName(QApplication::applicationDirPath(), false, "book_", "sqlite");
    QVERIFY(name2.startsWith("book_"));
    QVERIFY(name2.endsWith(".sqlite"));

    QString name4 = Utils::Rand::fileName(QApplication::applicationDirPath(), true, "book_", "sqlite");
    QVERIFY(name4.startsWith(QString("%1/book_").arg(QApplication::applicationDirPath())));
    QVERIFY(name4.endsWith(".sqlite"));

    // Custom file name prefix
    QString name5 = Utils::Rand::fileName(QApplication::applicationDirPath(), false, "mdb_", "sqlite");
    QVERIFY(name5.startsWith("mdb_"));
    QVERIFY(name5.endsWith(".sqlite"));

    QString name6 = Utils::Rand::fileName(QApplication::applicationDirPath(), true, "mdb_", "sqlite");
    QVERIFY(name6.startsWith(QString("%1/mdb_").arg(QApplication::applicationDirPath())));
    QVERIFY(name6.endsWith(".sqlite"));
}

void UtilsTest::arClean()
{
    QCOMPARE(Utils::String::Arabic::clean(_u("احمد")), _u("احمد"));

    QCOMPARE(Utils::String::Arabic::clean(_u("أحمد  ")), _u("احمد  "));

    QCOMPARE(Utils::String::Arabic::clean(_u("إِيَّاكَ نَعْبُدُ وَإِيَّاكَ نَسْتَعِينُ")),
             _u("اياك نعبد واياك نستعين"));

    QCOMPARE(Utils::String::Arabic::clean(_u("إِنَّ فِي ذَلِكَ لَآيَةً لِقَوْمٍ يَتَفَكَّرُونَ")),
             _u("ان في ذلك لايه لقوم يتفكرون"));
}

void UtilsTest::arCompare()
{
    QVERIFY(Utils::String::Arabic::compare(_u("وَتَرَى الْفُلْكَ مَوَاخِرَ فِيهِ وَلِتَبْتَغُوا مِنْ فَضْلِهِ وَلَعَلَّكُمْ تَشْكُرُونَ"),
                              _u("وتري الفلك مواخر فيه ولتبتغوا من فضله ولعلكم تشكرون")));
    QVERIFY(Utils::String::Arabic::compare(_u("قُلْ هُوَ اللَّهُ أَحَدٌ"),
                             _u("قل هُوَ الله احد")));
}

void UtilsTest::arContains()
{
    QVERIFY(Utils::String::Arabic::contains(_u("الله اكبر"), _u("الله")));
    QVERIFY(! Utils::String::Arabic::contains(_u("الله"), _u("الله اكبر")));
}

void UtilsTest::arFuzzyContains()
{
    QVERIFY(Utils::String::Arabic::fuzzyContains(_u("الله اكبر"), _u("الله")));
    QVERIFY(Utils::String::Arabic::fuzzyContains(_u("الله"), _u("الله اكبر")));
    QVERIFY(Utils::String::Arabic::fuzzyContains(_u("الله"), _u("قُلْ هُوَ اللَّهُ أَحَدٌ")));
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
            << "<ul dir=\"ltr\"><li><sanad>saad</sanad></li><li>kamal</li><li>karim</li><li><mateen>ahmed</mateen></li></ul>"
            << "No HTML tags";

    excpected << "Je suis \"kamal\" j'ai 15 < age > 18 "
              << "Test "
              << "My name is  kamal from  "
              << "saad   kamal  karim   ahmed   "
              << "No HTML tags";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::Html::removeTags(origins[i]));
}

void UtilsTest::getTags()
{
    QStringList origins;
    QStringList excpected;

    origins << "Sime test<sanad>This is a test</sanad>, test an other tag:"
               "<sanad>I mean <span>THIS</span> tag</sanad> if it work."
            << "Sime test<sanad id=\"n_2\" class=\"title\">This is a test</sanad>, test an other tag:"
               "<sanad style=\"color:red;\">I mean <span class=\"red\">THIS</span> tag</sanad> if it work."
            << "<SANAD CLASS=\"on\">Simple Text</SANAD>"
            << "<span class=\"red\">Simple Text</span>"
            << "No HTML tags";

    excpected << "This is a test I mean  THIS  tag "
              << "This is a test I mean  THIS  tag "
              << "Simple Text "
              << ""
              << "";

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
        QCOMPARE(Utils::Book::getPageTitleID(titles, pages[i]), excpectedTitles[i]);

}

void UtilsTest::getEmptyPageTitle()
{
    QList<int> titles;

    QCOMPARE(Utils::Book::getPageTitleID(titles, 15), 15);
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

void UtilsTest::sheerRegExp()
{
    QStringList origins;
    QList<bool> excpected;

    origins << _u("عجبت لشيخ يأمر الناس بالتقى ... وما راقب الرحمن يومًا وما اتقى")
            << _u("واشددْ يديكَ بحيلِ الدينِ معتصماً ... فإِنه الركنُ إِن خانَتْكَ أركانُ")
            << _u("وكثيرٌ من السؤالِ اشتياقٌ ... وكثيرٌ من ردِّه تعليلُ")
            << _u("لعل شَبَاً يفيدُ حباً ... فالشرُّ للخيرِ قد يجرُّ")
            << _u(" إِنما أولادُنا أكبادُنا ... وعلى الأَكْبادِ نحيا آملينْ.")
            << _u("يَبِيتُ يُجَافِي جَنْبَهُ عَنْ فِرَاشِهِ ... إِذَا اسْتَثْقَلَتْ بِالْمُشْرِكِينَ المَضَاجِعُ \"")

            << _u("هل ... ما تدري")
            << _u("ثم اتضح أن السبب في إخفاء ابن أبي العز، أو النساخ لاسمه، وهو الخوف من الهجمة الشرسة، التي كانت سائدة في عصره من قبل المخرفين، والمتعصبين، مؤيدين بقوة السلاطين الجاهلين ... الظاهر برقوق، وابنه الناصر فرج، ولاجين بن عبد الله الشركسي وأمثالهم، وكانوا على عقيدة سيئة، فضلا عما في سلوكهم من انحراف، وكانوا يقربون أصحاب وحدة الوجود، وأهل السحر، والزيج، وضرب الرمل, ولا تكاد تجد من المقربين إليهم إلا من اشتهر بذلك أو بما هو أسوأ!!");

    excpected << true
              << true
              << true
              << true
              << true
              << true

              << false
              << false;

    QRegExp rxSheer("^("
                    ".{15,50}"
                    " [\\.\\*]{3} "
                    ".{15,50}"
                    ")$");
//    QRegExp rxSheer("^([^\\.]+ \\.\\.\\. [^\\.]+)$");
    rxSheer.setMinimal(true);

    for(int i=0; i<origins.size(); i++) {
//        qDebug() << origins[i].indexOf("...");
        QVERIFY(excpected[i] == origins[i].contains(rxSheer));
    }
}

void UtilsTest::removeHTMLFormat()
{
    QStringList origins;
    QStringList excpected;

    origins << "<p>The head</p><p>Small paragraph<br />An other one</p><p>SecondHead</p>"
            << "<p>Line one<br />Line Two<br />Line Tree</p>"
            << "<p>The aya number<span class=\"ayanumber\"> (50) </span></span>"
               ":<span class=\"aya\"><br><span class=\"ayatxt\" id=\"s34a51\">aya</span></span></p>";

    excpected << "The head\n Small paragraph\nAn other one\n SecondHead\n"
              << "Line one\nLine Two\nLine Tree\n"
              << "The aya number  (50)   : \n aya  \n";

    for(int i=0; i<origins.size(); i++)
        QCOMPARE(excpected[i], Utils::Html::removeHTMLFormat(origins[i]));
}

void UtilsTest::cleanupTestCase()
{
    QSqlDatabase::removeDatabase("TestQueryBuilder");

    QVERIFY(QFile::remove(QDir::current().absoluteFilePath("test.db")));
}

void UtilsTest::findArabic()
{

    QStringList list1 = Utils::String::Arabic::getMatchString(_u("واترك ما حرم الله عليك واترك ما حرم الله عليك"),
                                                              _u("الله"));
    QCOMPARE(list1.size(), 2);
    QCOMPARE(list1[0], _u("الله"));
    QCOMPARE(list1[1], _u("الله"));

    QStringList list2 = Utils::String::Arabic::getMatchString(_u("الَّذِي جَعَلَ مَعَ اللَّهِ إِلَهًا آخَرَ "),
                                                              _u("الله"));
    QCOMPARE(list2.size(), 1);
    QCOMPARE(list2[0], _u("اللَّهِ"));

    QStringList list3 = Utils::String::Arabic::getMatchString(_u("وَجَاءَتْ سَكْرَةُ الْمَوْتِ بِالْحَقِّ ذَلِكَ مَا كُنْتَ مِنْهُ تَحِيدُ"),
                                                              _u("كنت منه تحيد"));

    QCOMPARE(list3.size(), 1);
    QCOMPARE(list3[0], _u("كُنْتَ مِنْهُ تَحِيدُ"));

    QStringList list4 = Utils::String::Arabic::getMatchString(_u("وَمِنْ شَرِّ حَاسِدٍ إِذَا حَسَدَ"),
                                                              _u("شر"));

    QCOMPARE(list4.size(), 1);
    QCOMPARE(list4[0], _u("شَرِّ"));

    QStringList list5 = Utils::String::Arabic::getMatchString(_u("العلمالعلم"),
                                                              _u("العلم"));

    QCOMPARE(list5.size(), 2);
    QCOMPARE(list5[0], _u("العلم"));
    QCOMPARE(list5[1], _u("العلم"));
}

void UtilsTest::cleanFileName()
{
    QCOMPARE(Utils::Files::cleanFileName("book first.pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book: first.pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book first.pdf", true),
             QString("book_first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book (first).pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book (first).pdf", true),
             QString("book_first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book [programming] (first).pdf", true),
             QString("book_programming_first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book <first>.pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book *first* ???.pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("book    first   .pdf"),
             QString("book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("/home/user/Documents/book <first>.pdf"),
             QString("/home/user/Documents/book first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("/home/user/Documents/book [programming] (123) <first >  .pdf", true),
             QString("/home/user/Documents/book_programming_123_first.pdf"));

    QCOMPARE(Utils::Files::cleanFileName("c:/home/user/Documents/book [programming] (123) <first >  .pdf", true),
             QString("c:/home/user/Documents/book_programming_123_first.pdf"));
}

void UtilsTest::footnoteRegExp()
{
    QString text1 = _u("دار النشر: دار الفكر العربي\nعدد الأجزاء: 10\nأعده "
            "للشاملة/ أبو إبراهيم حسانين، جزاه الله خيرا\n[ترقيم الشاملة موافق للمطبوع، وهو مذيل بالحواشي وضمن "
            "خدمة مقارنة التفاسير]\n__________\nتنبيه\n1 - صفحة 341 بها سطران ممسوحان في آخرها"
            "والكلمة الأخيرة من الصفحة التي تليها\nوفي"
            "صفحة 727 كلمة ممسوحة من السطر الرابع من أسفل\n2 - يوجد سقط من تفسير الإمام الجليل أبي زهرة من الآية 190 وحتى نهاية الآية 193 من سورة البقرة، وتميمًا للفائدة فقد نقل الناشر تفسير "
            "هذه الآيات من تفسير الإمام القرطبي.\n");

    QString text2 = _u("First line\n__________\n\nSecond line");

    QString text3 = _u("الاسعاف في احكام الاوقاف\n"
            "http://www.4shared.com/get/217350836/7137ec16/____.html\n\n"
            "الاشباه والنظائر\n"
            "http://www.4shared.com/get/233998481/12089cfc/______.html\n\n"
            "تحفة الناسك في احكام المناسك\n"
            "http://www.4shared.com/get/218171027/68c4599e/____________.html\n\n"
            "تغيير التنقيح في الاصول\n"
            "http://www.4shared.com/get/221050314/ca91402e/___.html\n\n"
            "تحفة الناسك في احكام المناسك\n"
            "http://www.4shared.com/get/218171027/68c4599e/____________.html\n"
            "تحفة الناسك في احكام المناسك\n"
            "http://www.4shared.com/get/218171027/68c4599e/____________.html\n");

    QString text4 = _u("[النساء: 163]" "\n"
                       "__________ W [ ش (أوحينا) أنزلنا عليك الرسالة");

    QString text5 = _u("عبدِ اللهِ بْنِ الْمُقَفَِّعِ (¬1)." "\n"
                       "¬_________" "\n"
                       "(¬1) كان عبد الله بن المقفع");
    // Separete footnote
    QRegExp footnoteSep(_u("\\n+¬?_{6,}[ \\n]+"));

    QCOMPARE(text1.split(footnoteSep, QString::SkipEmptyParts).size() , 2);
    QCOMPARE(text2.split(footnoteSep, QString::SkipEmptyParts).size() , 2);
    QCOMPARE(text3.split(footnoteSep, QString::SkipEmptyParts).size() , 1);
    QCOMPARE(text4.split(footnoteSep, QString::SkipEmptyParts).size() , 2);
    QCOMPARE(text5.split(footnoteSep, QString::SkipEmptyParts).size() , 2);

    QCOMPARE(text2.split(footnoteSep, QString::SkipEmptyParts).first(), _u("First line"));
    QCOMPARE(text2.split(footnoteSep, QString::SkipEmptyParts).last(), _u("Second line"));
}

QTEST_MAIN(UtilsTest)
