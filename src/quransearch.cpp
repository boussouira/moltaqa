#include "quransearch.h"
#include "ui_quransearch.h"

QuranSearch::QuranSearch(QWidget *parent, QSqlDatabase pDB) :
        QWidget(parent), m_ui(new Ui::QuranSearch), m_db(pDB)
{
    m_ui->setupUi(this);
    m_query = new QSqlQuery(m_db);
    m_resultModel = new QSqlQueryModel(this);

    connect(m_ui->searchButton, SIGNAL(clicked()), this, SLOT(searchForText()));
    connect(m_ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(gotoSora(QModelIndex)));

}

QuranSearch::~QuranSearch()
{
    delete m_ui;
}

void QuranSearch::searchForText()
{

    QList<QStandardItem *> listItems;
    QStringList indexsList;
    QStringList resultsList;
    QString indexsString;

    //m_resultModel->clear();
    m_query->prepare("SELECT id, Word, QURANTEXT_ID FROM QuranIndexs WHERE QuranIndexs.Word LIKE :text LIMIT 0, 20");
    m_query->bindValue(":text", QString("%%1%").arg(m_ui->lineEdit->text()));
    m_query->exec();

    { // Indexs list
    while (m_query->next())
        indexsList << QString("%1").arg(m_query->value(2).toString()).split(",");
    for (int i=0; i < indexsList.size(); i++)
        indexsString.append(QString("%1,").arg(indexsList.at(i).toLocal8Bit().constData()));
    indexsString.remove(QRegExp(",$"));
    indexsString.prepend("(");
    indexsString.append(")");
    }
    QString sqlQuery("SELECT QuranSowar.SoraName, QuranText.soraNumber, QuranText.ayaNumber, QuranText.ayaText "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.id IN ");
    sqlQuery.append(indexsString);
    
    m_resultModel->setQuery(sqlQuery, m_db);
    m_resultModel->setHeaderData(0, Qt::Horizontal, SORAT);
    m_resultModel->setHeaderData(1, Qt::Horizontal, RAKEM);
    m_resultModel->setHeaderData(2, Qt::Horizontal, AYANUMBER);
    m_resultModel->setHeaderData(3, Qt::Horizontal, ANASS);

    m_ui->tableView->setModel(m_resultModel);
    m_ui->tableView->resizeColumnsToContents();
    m_ui->tableView->setAlternatingRowColors(true);

    this->setResultCount(m_resultModel->rowCount(QModelIndex()));

    resultsList.clear();
}

void QuranSearch::gotoSora(QModelIndex pSelection)
{

    int selectedSora = m_resultModel->index(pSelection.row(), 1, QModelIndex()).data().toInt();
    int selectedAya  = m_resultModel->index(pSelection.row(), 2, QModelIndex()).data().toInt();

    emit resultSelected(selectedSora, selectedAya);

}

void QuranSearch::setResultCount(int pResultsCount)
{
    m_ui->groupBox->setTitle(QString(SEARCHRESULTS).arg(pResultsCount));
}
