#include "quransearch.h"
#include "ui_quransearch.h"

QuranSearch::QuranSearch(QWidget *parent, QSqlQuery *pQuery) :
        QWidget(parent), m_ui(new Ui::QuranSearch), m_query(pQuery)
{
    m_ui->setupUi(this);

    resultModel = new QStandardItemModel();

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

    resultModel->clear();
    m_query->prepare("SELECT id, Word, QURANTEXT_ID FROM QuranIndexs WHERE QuranIndexs.Word LIKE :text LIMIT 0, 20");
    m_query->bindValue(":text", QString("%%1%").arg(m_ui->lineEdit->text()));
    m_query->exec();

    { // Indexs list
    while (m_query->next())
        indexsList << QString("%1").arg(m_query->value(2).toString()).split(",");
    for (int i=0; i <= indexsList.size()-1; i++)
        indexsString.append(QString("%1,").arg(indexsList.at(i).toLocal8Bit().constData()));
    indexsString.remove(QRegExp(",$"));
    indexsString.prepend("(");
    indexsString.append(")");
    }
    QString sqlQuery("SELECT QuranSowar.SoraName, QuranText.ayaText, QuranText.soraNumber, QuranText.ayaNumber, QuranText.pageNumber "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.id IN ");
    sqlQuery.append(indexsString);
    m_query->exec(sqlQuery);

    while (m_query->next())
    {
        listItems.append(new QStandardItem(m_query->value(0).toString()));
        listItems.append(new QStandardItem(m_query->value(2).toString()));
        listItems.append(new QStandardItem(m_query->value(3).toString()));
        listItems.append(new QStandardItem(m_query->value(1).toString()));
        resultModel->appendRow(listItems);
        listItems.clear();
    }

    QStringList headersList;

    headersList << SORAT << RAKEM << AYANUMBER << ANASS;
    resultModel->setHorizontalHeaderLabels(headersList);
    m_ui->tableView->setModel(resultModel);
    m_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setResultCount(resultModel->rowCount(QModelIndex()));

    resultsList.clear();
}

void QuranSearch::gotoSora(QModelIndex pselection)
{

    int selectedSora = resultModel->index(pselection.row(), 1, QModelIndex()).data().toInt();
    int selectedAya  = resultModel->index(pselection.row(), 2, QModelIndex()).data().toInt();

    emit resultSelected(selectedSora, selectedAya);

}

void QuranSearch::setResultCount(int pResultsCount)
{
    m_ui->groupBox->setTitle(QString(SEARCHRESULTS).arg(pResultsCount));
}
