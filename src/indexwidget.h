#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "bookinfo.h"
#include <QDockWidget>
#include <QStringListModel>

namespace Ui {
    class IndexWidget;
}

class PageInfo;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    IndexWidget(QWidget *parent = 0);
    ~IndexWidget();
    void hideAyaSpin(bool visible);
    void hidePageSpin(bool visible);
    void hidePartSpin(bool visible);
    void setBookInfo(BookInfo *bInfo) { m_bookInfo = bInfo; }

protected:
    void changeEvent(QEvent *e);

public slots:
    void setIndex(QAbstractItemModel *pList);
    void displayBookInfo();
    void setSelectedSora(int pSoraNumber);
    void updatePageAndAyaNum(int pPageNumber, int pAyaNumber);
    int currentPageNmber();
    void updateAyaNumber(int pAyaNumber);

private slots:
    void ayaNumChange(int pAyaNum);
    void listDoubleClicked(QModelIndex pIndex);
    void openSoraInCurrentTab();
    void openSoraInNewTab();

signals:
    void ayaNumberChange(int pNewAyaNumber);
    void openSora(int pSoraNumber);
    void openSoraInNewTab(int pSoraNumber);
    void openPage(int pageID);

private:
    BookInfo *m_bookInfo;
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
