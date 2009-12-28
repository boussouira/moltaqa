#ifndef KTEXT_H
#define KTEXT_H

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QTextEdit>
#include <QMessageBox>
#include <QHash>
#include <QFile>

#include "constant.h"
#include "htmltemplates.h"

class KText
{
public:
    KText();
    void insertSoraName(QString pSoraName);
    void insertBassemala();
    void insertAyaText(QString pAyaText, int pAyaNumber, int pSoraNumber);
    QString text();
    void clear();

private:
    QString m_text;

};

#endif // KTEXT_H
