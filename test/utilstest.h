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
};

#endif // UTILSTEST_H
