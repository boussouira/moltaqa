#ifndef QURANSEARCH_H
#define QURANSEARCH_H

#include <QtGui/QWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>
#include <QList>
#include <QDebug>
#include <QStandardItemModel>

#include "constant.h"

namespace Ui {
    class QuranSearch;
}

class QuranSearch : public QWidget {
    Q_OBJECT
public:
    QuranSearch(QWidget *parent = 0, QSqlQuery *pQuery = 0);
    ~QuranSearch();
signals:
    void resultSelected(int sora, int aya);

private slots:
    void searchForText();
    void gotoSora(QModelIndex pselection);
    void setResultCount(int pResultsCount);
private:
    Ui::QuranSearch *m_ui;
    QSqlQuery *m_query;
    QStandardItemModel *resultModel;

};

#endif // QURANSEARCH_H
