#ifndef INDEXDOCKWIDGET_H
#define INDEXDOCKWIDGET_H

#include <QDockWidget>
#include <QStringListModel>

namespace Ui {
    class IndexDockWidget;
}

class PageInfo;

class IndexDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    IndexDockWidget(QWidget *parent = 0);
    ~IndexDockWidget();

protected:
    void changeEvent(QEvent *e);

public slots:
    void setIndex(QStringListModel *pList);
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

private:
    Ui::IndexDockWidget *ui;
    bool sendSignals;
};

#endif // INDEXDOCKWIDGET_H
