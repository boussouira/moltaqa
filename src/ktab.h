#ifndef KTAB_H
#define KTAB_H

#include <QTabWidget>
#include <QWebView>
#include <QVBoxLayout>
#include <QTabBar>
#include <QDebug>

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
    void tabIsMoved(int from, int to);

signals:
    void reloadCurrentSoraInfo();
    void tabMoved(int from, int to);

private:
    QList<SoraInfo* > m_sowarInfo;
    QTabBar *m_tab;

};

#endif // KTAB_H
