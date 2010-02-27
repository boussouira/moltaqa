#ifndef KMODEL_H
#define KMODEL_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>

class SoraInfo;
class KText;
class QuranModel : public QObject
{
    Q_OBJECT
public:
    explicit QuranModel(QObject *parent, QString pQuranDBPath);
    ~QuranModel();
public slots:
    void getSowarList(QStringListModel *pSowarModel);
    void getSoraInfo(int pSoraNumber, int pAyaNumber, SoraInfo *pSoraInfo);
    int getAyaPageNumber(int pSoraNumber, int pAyaNumber);
    QString getQuranPage(SoraInfo *pSoraInfo);

private:
    KText *m_text;
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    QString m_quranDBPath;

};

#endif // KMODEL_H

