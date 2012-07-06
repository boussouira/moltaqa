#ifndef UTILSTEST_H
#define UTILSTEST_H

#include <QtTest/QtTest>

class UtilsTest : public QObject
{
    Q_OBJECT
public:
    UtilsTest();

private slots:
    void randInt();
    void generateFileName();
    void arClean();
    void arCompare();
    void arContains();
    void arFuzzyContains();

    void removeHtmlSpecialChars();
    void htmlSpecialCharsEncode();
    void removeHtmlTags();
    void getTags();

    void getPageTitle();

    void formatHTML();

    void queryBuilder();

    void sheerRegExp();

    void removeHTMLFormat();
    void cleanupTestCase();

    void findArabic();

protected:
    int getPageTitleID(QList<int> &titles, int pageID);
};

#endif // UTILSTEST_H
