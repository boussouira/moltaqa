#include "quransearch.h"

QuranSearch::QuranSearch(QWidget *parent, QString pQuranDbPath) :
        QWidget(parent), m_ui(new Ui::QuranSearch)
{
    m_ui->setupUi(this);
    m_db = QSqlDatabase::addDatabase("QSQLITE", "QuranDBSearchl");
    m_db.setDatabaseName(pQuranDbPath);
    if (!m_db.open()) {
        qFatal("Cannot open database.");
    }
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

    QString indexsString;
    QStringList wordsList = this->spiltText(m_ui->lineEdit->text());

    for (int i=0; i<=wordsList.count()-1; i++ )
    {
        indexsString.append(QString("%1 QuranText.id IN %2 ").arg((i) ? "AND" : "WHERE").arg(this->getIdsList(wordsList.at(i))));
    }

    QString sqlQuery("SELECT QuranSowar.SoraName, QuranText.soraNumber, QuranText.ayaNumber, QuranText.ayaText "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber ");
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

QStringList QuranSearch::spiltText(QString str)
{
    QStringList list;
    QRegExp regEx("([^ ]+)");
    int pos = 0;

    while ((pos = regEx.indexIn(str, pos)) != -1) {
        list << regEx.cap(1);
        pos += regEx.matchedLength();
    }

    return list;

}

bool QuranSearch::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Return) {
            this->searchForText();
            return true;
        }
    }

    return QWidget::event(event);
}

