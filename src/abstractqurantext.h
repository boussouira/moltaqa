#ifndef ABSTRACTQURANTEXT_H
#define ABSTRACTQURANTEXT_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>

class AbstractQuranText : public QObject
{
    Q_OBJECT
public:
    AbstractQuranText(QObject *parent);
public:
    /**
      @brief Open the Quran database.
      @param pQuranDBPath   Quran database path.
      */
    void openQuranDB(QString pQuranDBPath);
    /**
      @brief Get all SOWAR names as QStringListModel.
      @param pSowarModel    Pointer to a QStringListModel.
      */
    void getSowarList(QStringListModel *pSowarModel);
protected:
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    QString m_quranDBPath;
};

#endif // ABSTRACTQURANTEXT_H
