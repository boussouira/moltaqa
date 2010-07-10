#include "quransearch.h"

QuranSearch::QuranSearch(QWidget *parent, QString pQuranDbPath) :
        QWidget(parent), m_ui(new Ui::QuranSearch)
{
    m_ui->setupUi(this);
    m_db = QSqlDatabase::addDatabase("QSQLITE", "QuranDBSearchl");
    m_db.setDatabaseName(pQuranDbPath);
    if (!m_db.open()) {
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    }
    m_query = new QSqlQuery(m_db);
    m_resultModel = new QSqlQueryModel(this);

    connect(m_ui->searchButton, SIGNAL(clicked()), this, SLOT(searchForText()));
    connect(m_ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(gotoSora(QModelIndex)));

}

QuranSearch::~QuranSearch()
{
    delete m_ui;
    delete m_query;
}

void QuranSearch::searchForText()
{

    QString indexsString;
    QStringList wordsList = m_ui->lineEdit->text().split(" ", QString::SkipEmptyParts);

    for (int i=0; i<=wordsList.count()-1; i++ )
    {
        indexsString.append(QString("%1 QuranText.id IN %2 ").arg((i) ? "AND" : "WHERE").arg(getIdsList(wordsList.at(i))));
    }

    QString sqlQuery("SELECT QuranSowar.SoraName, QuranText.soraNumber, QuranText.ayaNumber, QuranText.ayaText "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber ");
    sqlQuery.append(indexsString);
    
    m_resultModel->setQuery(sqlQuery, m_db);
    m_resultModel->setHeaderData(0, Qt::Horizontal, trUtf8("سورة"));
    m_resultModel->setHeaderData(1, Qt::Horizontal, trUtf8("رقم"));
    m_resultModel->setHeaderData(2, Qt::Horizontal, trUtf8("الاية"));
    m_resultModel->setHeaderData(3, Qt::Horizontal, trUtf8("النص"));

    m_ui->tableView->setModel(m_resultModel);
    m_ui->tableView->resizeColumnsToContents();
    m_ui->tableView->setAlternatingRowColors(true);

    setResultCount(m_resultModel->rowCount(QModelIndex()));

}

void QuranSearch::gotoSora(QModelIndex pSelection)
{

    int selectedSora = m_resultModel->index(pSelection.row(), 1, QModelIndex()).data().toInt();
    int selectedAya  = m_resultModel->index(pSelection.row(), 2, QModelIndex()).data().toInt();

    emit resultSelected(selectedSora, selectedAya);

}

void QuranSearch::setResultCount(int pResultsCount)
{
    m_ui->groupBox->setTitle(trUtf8("نتائج البحث %1 :").arg(pResultsCount));
}

QString QuranSearch::getIdsList(QString pWord)
{
    QStringList indexsList;
    QString indexsString;

    m_query->prepare("SELECT id, Word, QURANTEXT_ID FROM QuranIndexs WHERE QuranIndexs.Word LIKE :text LIMIT 0, 20");
    m_query->bindValue(":text", QString("%%1%").arg(pWord));
    m_query->exec();

    // Indexs list
    while (m_query->next())
        indexsList << QString("%1").arg(m_query->value(2).toString()).split(",");
    for (int i=0; i < indexsList.size(); i++)
        indexsString.append(QString("%1,").arg(indexsList.at(i).toLocal8Bit().constData()));
    indexsString.remove(QRegExp(",$"));
    indexsString.prepend("(");
    indexsString.append(")");

    return indexsString;
}

bool QuranSearch::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Return) {
            searchForText();
            return true;
        }
    }

    return QWidget::event(event);
}

