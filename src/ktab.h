#ifndef KTAB_H
#define KTAB_H

#include <QTabWidget>
#include <QWebView>
#include <QVBoxLayout>

#include "constant.h"
#include "sorainfo.h"

class KTab : public QTabWidget
{
    Q_OBJECT
public:
    KTab(QWidget *parent = 0);

public slots:
    void addNewOnglet();
    void closeTab(int tabIndex);
    QWidget *newOnglet();
    QWebView *currentPage();
    SoraInfo *currentSoraInfo();

signals:
    void reloadCurrentSoraInfo();

private:
    QList<SoraInfo* > m_sowarInfo;

};

#endif // KTAB_H
