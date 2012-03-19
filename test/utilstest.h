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
};

#endif // UTILSTEST_H
