#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_textCrusor = new CTextCrusor(ui->textBrowser);
    m_sora = new SoraInfo();
    LoadSettings();

    m_db = QSqlDatabase::addDatabase("QSQLITE", "QuranDB");
    m_db.setDatabaseName(m_databasePATH);

    if (!m_db.open()) {
        QMessageBox::critical(this,
                              tr("Cannot open database"),
                              tr("Unable to establish a database connection."));
        exit(1);
    }
     m_sowarNamesModel = new QStringListModel();
     QStringList sowarNamesList;
     m_query = new QSqlQuery(m_db);
     m_query->exec("SELECT * FROM QuranSowar");
      while (m_query->next())
      {
           sowarNamesList << QString("%1. %2")
                   .arg(m_query->value(0).toString())
                   .arg(m_query->value(1).toString() );
      }
      
     m_sowarNamesModel->setStringList(sowarNamesList);
     ui->listView->setModel(m_sowarNamesModel);

     m_search = new QuranSearch(this, m_query);
    ui->verticalLayout_5->addWidget(m_search);

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectedSoraChange(QModelIndex)));
   // connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(pageNumberChange(int)));
    connect(ui->spinBoxAyaNumber, SIGNAL(valueChanged(int)), this, SLOT(ayaNumberChange(int)));
    connect(m_search, SIGNAL(resultSelected(int,int)), this, SLOT(selectResult(int,int)));

    connect(ui->actionSelect_all, SIGNAL(triggered()), ui->textBrowser, SLOT(selectAll()));
    connect(ui->actionCopy, SIGNAL(triggered()), ui->textBrowser, SLOT(copy()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionChangeFont, SIGNAL(triggered()), this, SLOT(textChangeFont()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutQR()));


}

MainWindow::~MainWindow()
{

    settings->setValue("textBrowser/font", ui->textBrowser->font().toString());
//    settings->setValue("textBrowser/size", ui->textBrowser->font().pointSize());
    settings->setValue("app/db", m_databasePATH);

    delete ui;
}

void MainWindow::selectSora(int psoraNumber, int payaNumber, bool pDisplay)
{
    m_textCrusor->clearAll();
    m_sora->setNumber(psoraNumber);

    m_query->prepare("SELECT SoraName, ayatCount, SoraDescent "
                     "FROM QuranSowar "
                     "WHERE QuranSowar.id = :id LIMIT 1");

    m_query->bindValue(":id", m_sora->number());
    m_query->exec();

    if (m_query->first()){
        m_sora->setName(m_query->value(0).toString());
        m_sora->setAyatCount(m_query->value(1).toInt());
        m_sora->setDescent(m_query->value(2).toString());
    }

    m_query->prepare("SELECT MIN(QuranText.pageNumber) AS MinPNum, MAX(QuranText.pageNumber) AS MaxPNum "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :m_currentSoraNumber ");

    m_query->bindValue(":m_currentSoraNumber", m_sora->number());
    m_query->exec();

    if(m_query->first())
    {
        m_sora->setCurrentPage(m_query->value(0).toInt());
    }

    if(pDisplay)
        this->display(m_sora->currentPage(), m_sora->number(), payaNumber);
    this->setSoraDetials();
    ui->spinBoxPageNumber->setValue(m_sora->currentPage());

}

void MainWindow::setSoraDetials()
{
    QTableWidgetItem *itemSoraName      = new QTableWidgetItem(m_sora->name());
    QTableWidgetItem *itemSoraNumber    = new QTableWidgetItem(QString::number(m_sora->number()));
    QTableWidgetItem *itemSoraAyatCount = new QTableWidgetItem(QString::number(m_sora->ayatCount()));
    QTableWidgetItem *itemSoraDescent   = new QTableWidgetItem(m_sora->descent());

    ui->tableWidget->setItem(0, 1, itemSoraName);
    ui->tableWidget->setItem(1, 1, itemSoraNumber);
    ui->tableWidget->setItem(2, 1, itemSoraAyatCount);
    ui->tableWidget->setItem(3, 1, itemSoraDescent);

    // set ayat count in the spin box
    ui->spinBoxAyaNumber->setMaximum(m_sora->ayatCount());
    ui->spinBoxAyaNumber->setSuffix(QString(" / %1").arg(m_sora->ayatCount()));
    ui->spinBoxAyaNumber->setValue(1);

    // Select sora in the list view
    this->setSelectedSora(m_sora->number());
}

void MainWindow::display(int pPageNumber, int pSoraNumber, int pAyaNumber)
{
    m_textCrusor->clearAll();

    m_query->prepare("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                     "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.pageNumber = :m_pageNumber "
                     "ORDER BY QuranText.id ");

    m_query->bindValue(":m_pageNumber", pPageNumber);
    m_query->exec();
    m_textCrusor->setSoraAndAyaToHighLight(pSoraNumber, pAyaNumber);

    m_sora->setCurrentAya(pAyaNumber);
    ui->spinBoxAyaNumber->setValue(pAyaNumber);

    while (m_query->next())
    {
        if(m_query->value(2).toInt() == 1) // at the first vers we insert the sora name and bassemala
        {
            m_textCrusor->insertSoraName(m_query->value(5).toString());
            if(pSoraNumber != 1 and pSoraNumber != 9) // we escape putting bassemala before Fateha and Tawba
            {
                m_textCrusor->insertBassemala();
            }
        }
        m_textCrusor->insertAyaText(m_query->value(1).toString(),
                                     m_query->value(2).toInt(),
                                     m_query->value(4).toInt());
    }
    this->scrollToAya(pSoraNumber, pAyaNumber);
}
/*
void MainWindow::pageNumberChange(int pNewPageNumbe)
{

    int tmpFirstAyaInPage;
    int tmpLastAyaInPage;
    int tmpFirstSoraInPage;
    int tmpLastSoraInPage;

    this->getFirsLastAyaNumberInPage(m_currentSoraNumber, pNewPageNumbe,
                                     &tmpFirstAyaInPage, &tmpLastAyaInPage);
    this->getFirsLastSoraNumberInPage(pNewPageNumbe, &tmpFirstSoraInPage, &tmpLastSoraInPage);

    if (m_currentPageNumber == pNewPageNumbe)
        return ;

    m_currentPageNumber = pNewPageNumbe;
    if((tmpFirstAyaInPage > m_currentAyaNumber) or (tmpLastAyaInPage < m_currentAyaNumber))
    {
        m_currentAyaNumber = tmpFirstAyaInPage;
        ui->spinBoxAyaNumber->setValue(tmpFirstAyaInPage);
    }
    if((tmpFirstSoraInPage > m_currentSoraNumber) or (tmpLastSoraInPage < m_currentSoraNumber))
    {
        this->selectSora(tmpFirstSoraInPage,1, false);
    }
    this->display(m_currentPageNumber, m_currentSoraNumber, tmpFirstAyaInPage);

}
*/

void MainWindow::ayaNumberChange(int pNewAyaNumber)
{
    if (m_sora->currentAya() == pNewAyaNumber)
        return ;
    int tmpPageNumber = this->getAyaPageNumber(m_sora->number(), pNewAyaNumber);

    if(tmpPageNumber != m_sora->currentPage())
    {
        m_sora->setCurrentAya(pNewAyaNumber);
        m_sora->setCurrentPage(tmpPageNumber);
        this->display(m_sora->currentPage(), m_sora->number(), m_sora->currentAya());

        ui->spinBoxPageNumber->setValue(tmpPageNumber);
    }
    this->scrollToAya(m_sora->number(), pNewAyaNumber);

}

void MainWindow::selectedSoraChange(QModelIndex pselection)
{
    m_sora->setNumber(pselection.row() + 1);
    this->selectSora(m_sora->number());
}

int MainWindow::getAyaPageNumber(int pSoraNumber, int pAyaNumber)
{
    m_query->prepare("SELECT QuranText.pageNumber "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :s_soraNumber "
                     "AND QuranText.ayaNumber = :s_ayaNumber "
                     "LIMIT 1 ");

    m_query->bindValue(":s_soraNumber", pSoraNumber);
    m_query->bindValue(":s_ayaNumber", pAyaNumber);
    m_query->exec();

    if(m_query->first())
    {
        return m_query->value(0).toInt();
    }
    return 1;
}
/*
void MainWindow::getFirsLastAyaNumberInPage(int pSoraNumber, int pPageNumber, int *pFirstAya, int *pLastAya)
{
    m_query->prepare("SELECT MIN(QuranText.ayaNumber) AS FirstAyaInPage, "
                     "MAX(QuranText.ayaNumber) AS LastAyaInPage "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :s_soraNumber "
                     "AND QuranText.pageNumber = :s_pageNumber "
                     "LIMIT 1 ");

    m_query->bindValue(":s_soraNumber", pSoraNumber);
    m_query->bindValue(":s_pageNumber", pPageNumber);
    m_query->exec();

    if(m_query->first())
    {
        *pFirstAya = m_query->value(0).toInt();
        *pLastAya  = m_query->value(1).toInt();
    }
}

void MainWindow::getFirsLastSoraNumberInPage(int pPageNumber, int *pFirstSoraNumber, int *pLastSoraNumber)
{
    m_query->prepare("SELECT MIN(QuranText.soraNumber) AS FirstSoraInPage, "
                     "MAX(QuranText.soraNumber) AS LastSoraInPage "
                     "FROM QuranText "
                     "WHERE QuranText.pageNumber = :s_pageNumber "
                     "LIMIT 1 ");
    m_query->bindValue(":s_pageNumber", pPageNumber);
    m_query->exec();

        if(m_query->first())
    {
        *pFirstSoraNumber = m_query->value(0).toInt();
        *pLastSoraNumber  = m_query->value(1).toInt();
    }
}
*/
void MainWindow::textChangeFont()
{
    ui->textBrowser->setFont(QFontDialog::getFont(0, ui->textBrowser->font()));
}

void MainWindow::aboutQR()
{
    QMessageBox::information(this, "About", "Quran Reader");
}

void MainWindow::LoadSettings()
{
    QFont font;
    settings = new QSettings(CONFIGFILE, QSettings::IniFormat, this);

    QString fontName = settings->value("textBrowser/font", QVariant(DEFAUTFONT)).toString();
    m_databasePATH = settings->value("app/db", QVariant(DATABASEPATH)).toString();
    font.fromString(fontName);
    ui->textBrowser->setFont(font);

    while (!QFile::exists(m_databasePATH))
    {
        int rep = QMessageBox::critical(this,
                              tr("Cannot open database"),
                              tr("Unable to establish a database connection\n"
                                 "You need to select database."),
                              QMessageBox::Yes | QMessageBox::No);
        if (rep == QMessageBox::No)
            exit(1);
         m_databasePATH = QFileDialog::getOpenFileName(this,
                                                       tr("Select the database"), "",
                                                       tr("SQLITE (*.db);;All files (*.*)"));
         if(!m_databasePATH.isEmpty())
            break;
    }
}
 
void MainWindow::scrollToAya(int pSoraNumber, int pAyaNumber)
{
    ui->textBrowser->setTextCursor(m_textCrusor->getCursorPosition(pSoraNumber, pAyaNumber));
}

void MainWindow::selectResult(int pSoraNumber, int pAyaNumber)
{
    int pageNumber = this->getAyaPageNumber(pSoraNumber, pAyaNumber);

    this->selectSora(pSoraNumber, pAyaNumber, false);
    this->display(pageNumber, pSoraNumber, pAyaNumber);
//    ui->spinBoxAyaNumber->setValue(pAyaNumber);
}

void MainWindow::setSelectedSora(int pSoraNumber)
{
    QItemSelectionModel *selection = ui->listView->selectionModel();
    QModelIndex indexElementSelectionne = selection->currentIndex();
    QModelIndex itemToSelect = m_sowarNamesModel->index(pSoraNumber - 1, 0, QModelIndex());
    selection->select(itemToSelect, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->listView->scrollTo(itemToSelect);

}
