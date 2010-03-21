#ifndef BOOKSVIEWER_H
#define BOOKSVIEWER_H

#include <QMainWindow>
#include <QtWebKit>
#include "ktab.h"
#include "indexdockwidget.h"
#include "qurantextmodel.h"
#include "tafessirtextbase.h"

class BooksViewer : public QWidget
{
    Q_OBJECT
public:
    BooksViewer(QMainWindow *parent = 0);
    ~BooksViewer();

public slots:
    void openSora(int pSoraNumber, int pAyaNumber = 1);
    void displayPage(PageInfo *pPageInfo);
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void openSoraInNewTab(int pSoraNumber = 1);
    void ayaNumberChange(int pNewAyaNum);
    void updateSoraDetials();
    void nextAya();
    void previousAYA();
    void nextPage();
    void previousPage();
    inline int showNextPageButton() const {return m_showNextPageButton; }
    inline int showPrevPageButton() const {return m_showPrevPageButton; }
    void showIndexDock(bool pShowIndexDock);
private:
    void navigationButtonsStat();

signals:
    void updateNavigationButtons();

private:
    KTab *m_tab;
    IndexDockWidget *m_indexDock;
    QuranTextModel *m_quranModel;
    TafessirTextBase *m_tafessir;
    bool m_showNextPageButton;
    bool m_showPrevPageButton;
};

#endif // BOOKSVIEWER_H
