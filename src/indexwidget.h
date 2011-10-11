#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "librarybook.h"
#include <qdockwidget.h>
#include <qstringlistmodel.h>

namespace Ui {
    class IndexWidget;
}

class BookPage;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    IndexWidget(QWidget *parent = 0);
    ~IndexWidget();
    void hideAyaSpin(bool visible);
    void hidePageSpin(bool visible);
    void hidePartSpin(bool visible);
    void setBookInfo(LibraryBook *bInfo) { m_bookInfo = bInfo; }

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
    void openSora(int sora, int aya);
    void openSoraInNewTab(int pSoraNumber);
    void openPage(int pageID);

private:
    LibraryBook *m_bookInfo;
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
