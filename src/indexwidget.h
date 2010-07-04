#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

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

protected:
    void changeEvent(QEvent *e);

public slots:
    void setIndex(QAbstractItemModel *pList);
    void setSoraDetials(PageInfo *pPageInfo);
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
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
