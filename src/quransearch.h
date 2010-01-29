#ifndef QURANSEARCH_H
#define QURANSEARCH_H

#include <QtGui/QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QStringListModel>
#include <QList>
#include <QStandardItemModel>

#include "constant.h"

namespace Ui {
    class QuranSearch;
}

class QuranSearch : public QWidget {
    Q_OBJECT
public:
    QuranSearch(QWidget *parent, QSqlDatabase pDB);
    ~QuranSearch();
signals:
    void resultSelected(int sora, int aya);

private slots:
    void searchForText();
    void gotoSora(QModelIndex pSelection);
    void setResultCount(int pResultsCount);
    QString getIdsList(QString pWord);
    QStringList spiltText(QString str);
private:
    Ui::QuranSearch *m_ui;
    QSqlDatabase m_db;
    QSqlQuery *m_query;
    QSqlQueryModel *m_resultModel;

};

#endif // QURANSEARCH_H
