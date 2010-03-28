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
#include <QKeyEvent>

#include "ui_quransearch.h"

namespace Ui {
    class QuranSearch;
}

/**
  @brief Simple search in The Quran database.
  */
class QuranSearch : public QWidget {
    Q_OBJECT
public:
    /**
      @brief The constructor.
      @param parent         The parent widget.
      @param pQuranDbPath   The Quran database path.
      */
    QuranSearch(QWidget *parent, QString pQuranDbPath);

    /// @brief The destructor.
    ~QuranSearch();
protected:
    /**
    @brief Simple event handler.
      */
    bool event(QEvent *event);

signals:
    /**
      @brief This signal is emited when the user select an item on the QTableView.
      @param sora   Selected SORA number.
      @param aya    Selected AYA number.
      */
    void resultSelected(int sora, int aya);

protected slots:
    /**
      @brief Search in the database for the text in the QLineEdit
             and display the results in the QTableView.
      @todo Improve this search method.
      */
    void searchForText();

    /**
      @brief Get the selected SORA and AYA number from
             the QTableView and emit the signal resultSelected().
      @param pSelection     Selected item index.
      @see resultSelected
      */
    void gotoSora(QModelIndex pSelection);

    /**
      @brief Display the results count.
      @param pResultsCount      Results count.
      */
    void setResultCount(int pResultsCount);

    /**
      @brief Search in the QuranIndexs table for a root (word), and get the QURANTEXT_ID entries
             in order to use this IDs to get the results from the QuranText table.
      @param pWord      The root word.
      */
    QString getIdsList(QString pWord);

private:
    Ui::QuranSearch *m_ui;
    QSqlDatabase m_db;
    QSqlQuery *m_query;
    QSqlQueryModel *m_resultModel;

};

#endif // QURANSEARCH_H

