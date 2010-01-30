#include "mainwindow.h"

#include <QtWebKit>
#include <QSettings>
#include <QStringListModel>
#include <QMessageBox>

#include "constant.h"
#include "ktext.h"
#include "quransearch.h"
#include "sorainfo.h"
#include "settings.h"
#include "ktab.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    
    ui->setupUi(this);

    m_text = new KText();
    m_settings = new Settings(this);

    this->loadSettings();
    this->setupActions();
    this->setupDataBases();
    this->setupQuranIndex();

    m_tab = new KTab(ui->centralWidget);
    ui->verticalLayout_4->addWidget(m_tab);

    m_search = new QuranSearch(this, m_db);
    ui->verticalLayout_5->addWidget(m_search);

    m_tab->addNewOnglet();

    this->setupConnections();

}

void MainWindow::setupActions()
{
    QAction *actionOpenSora = new QAction(OPENSORA, ui->listView);
    QAction *actionOpenSoraInNewTab = new QAction(OPENSORAINTAB, this);
    ui->listView->addAction(actionOpenSora);
    ui->listView->addAction(actionOpenSoraInNewTab);

    connect(actionOpenSora, SIGNAL(triggered()), this, SLOT(openSora()));
    connect(actionOpenSoraInNewTab, SIGNAL(triggered()), this, SLOT(openSoraInNewTab()));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAlKotobiya()));
    connect(ui->actionNewTab, SIGNAL(triggered()), this, SLOT(addNewTab()));


}

void MainWindow::setupConnections()
{
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openSora(QModelIndex)));
    connect(ui->spinBoxAyaNumber, SIGNAL(valueChanged(int)), this, SLOT(ayaNumberChange(int)));
    connect(m_search, SIGNAL(resultSelected(int,int)), this, SLOT(selectResult(int,int)));

    connect(ui->dockIndexs, SIGNAL(visibilityChanged(bool)), this, SLOT(hideDockIndex()));
    connect(ui->dockSearch, SIGNAL(visibilityChanged(bool)), this, SLOT(hideDockSearch()));
    connect(ui->actionIndexDock, SIGNAL(toggled(bool)), ui->dockIndexs, SLOT(setShown(bool)));
    connect(ui->actionSearchDock, SIGNAL(toggled(bool)), ui->dockSearch, SLOT(setShown(bool)));
    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(reloadSoraInfo()));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), m_tab, SLOT(closeTab(int)));
    connect(m_tab, SIGNAL(reloadCurrentSoraInfo()), this, SLOT(reloadSoraInfo()));
}

void MainWindow::setupDataBases()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "QuranDB");
    m_db.setDatabaseName(m_settings->dbPath());

    if (!m_db.open()) {
        QMessageBox::critical(this,
                              tr("Cannot open database"),
                              tr("Unable to establish a database connection."));
        exit(1);
    }
    m_query = new QSqlQuery(m_db);
}

void MainWindow::setupQuranIndex()
{
    m_sowarNamesModel = new QStringListModel();
    QStringList sowarNamesList;
    m_query->exec("SELECT * FROM QuranSowar");
    while (m_query->next())
    {
      sowarNamesList << QString("%1. %2")
              .arg(m_query->value(0).toString())
              .arg(m_query->value(1).toString() );
    }

    m_sowarNamesModel->setStringList(sowarNamesList);
    ui->listView->setModel(m_sowarNamesModel);

}

void MainWindow::hideDockIndex()
{
    ui->actionIndexDock->setChecked(!ui->dockIndexs->isHidden());
}

void MainWindow::hideDockSearch()
{
    ui->actionSearchDock->setChecked(!ui->dockSearch->isHidden());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectSora(int pSoraNumber, int pAyaNumber, bool pDisplay)
{
    m_tab->currentSoraInfo()->setNumber(pSoraNumber);

    m_query->prepare("SELECT SoraName, ayatCount, SoraDescent "
                     "FROM QuranSowar "
                     "WHERE QuranSowar.id = :id LIMIT 1");

    m_query->bindValue(":id", m_tab->currentSoraInfo()->number());
    m_query->exec();

    if (m_query->first()){
        m_tab->currentSoraInfo()->setName(m_query->value(0).toString());
        m_tab->currentSoraInfo()->setAyatCount(m_query->value(1).toInt());
        m_tab->currentSoraInfo()->setDescent(m_query->value(2).toString());
    }

    m_query->prepare("SELECT MIN(QuranText.pageNumber) AS MinPNum, MAX(QuranText.pageNumber) AS MaxPNum "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :m_currentSoraNumber ");

    m_query->bindValue(":m_currentSoraNumber", m_tab->currentSoraInfo()->number());
    m_query->exec();

    if(m_query->first())
    {
        m_tab->currentSoraInfo()->setCurrentPage(m_query->value(0).toInt());
    }

    if(pDisplay)
        this->display(m_tab->currentSoraInfo()->currentPage(), m_tab->currentSoraInfo()->number(), pAyaNumber);
    this->setSoraDetials();
    ui->spinBoxPageNumber->setValue(m_tab->currentSoraInfo()->currentPage());

}

void MainWindow::setSoraDetials()
{
    freez = true;

    // Set AYAT count and current AYA number in the spin box
    ui->spinBoxAyaNumber->setMaximum(m_tab->currentSoraInfo()->ayatCount());
    ui->spinBoxAyaNumber->setSuffix(QString(" / %1").arg(m_tab->currentSoraInfo()->ayatCount()));
    ui->spinBoxAyaNumber->setValue(m_tab->currentSoraInfo()->currentAya());

    this->setSelectedSora(m_tab->currentSoraInfo()->number());
    freez = false;
}

void MainWindow::display(int pPageNumber, int pSoraNumber, int pAyaNumber)
{
    m_text->clear();

    m_query->prepare("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                     "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.pageNumber = :m_pageNumber "
                     "ORDER BY QuranText.id ");

    m_query->bindValue(":m_pageNumber", pPageNumber);
    m_query->exec();

    m_tab->currentSoraInfo()->setCurrentAya(pAyaNumber);
    ui->spinBoxAyaNumber->setValue(pAyaNumber);

    while (m_query->next())
    {
        if(m_query->value(2).toInt() == 1) // at the first vers we insert the sora name and bassemala
        {
            m_text->insertSoraName(m_query->value(5).toString());
            if(pSoraNumber != 1 and pSoraNumber != 9) // we escape putting bassemala before Fateha and Tawba
            {
                m_text->insertBassemala();
            }
        }
        m_text->insertAyaText(m_query->value(1).toString(),
                                     m_query->value(2).toInt(),
                                     m_query->value(4).toInt());
    }

    m_tab->currentPage()->page()->mainFrame()->setHtml(m_text->text());
    m_tab->setTabText(m_tab->currentIndex(), QString("%1 %2").arg(SORAT).arg(m_tab->currentSoraInfo()->name()));
    this->scrollToAya(pSoraNumber, pAyaNumber);
}

void MainWindow::ayaNumberChange(int pNewAyaNumber)
{
    // If freez == true then break.
    if (m_tab->currentSoraInfo()->currentAya() == pNewAyaNumber or freez)
        return ;
    int page = this->getAyaPageNumber(m_tab->currentSoraInfo()->number(), pNewAyaNumber);

    if(page != m_tab->currentSoraInfo()->currentPage())
    {
        m_tab->currentSoraInfo()->setCurrentPage(page);
        this->display(m_tab->currentSoraInfo()->currentPage(), m_tab->currentSoraInfo()->number(), m_tab->currentSoraInfo()->currentAya());

        ui->spinBoxPageNumber->setValue(page);
    }

    m_tab->currentSoraInfo()->setCurrentAya(pNewAyaNumber);
    this->scrollToAya(m_tab->currentSoraInfo()->number(), pNewAyaNumber);

}

void MainWindow::openSora(QModelIndex pSelection)
{
    if(!freez)
    {
        m_tab->currentSoraInfo()->setNumber(pSelection.row() + 1);
        this->selectSora(m_tab->currentSoraInfo()->number());
    }
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

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this, "About", "Al Kotobiya");
}

void MainWindow::loadSettings()
{
    freez = false;
}
 
void MainWindow::scrollToAya(int pSoraNumber, int pAyaNumber)
{

    QWebFrame *frame = m_tab->currentPage()->page()->mainFrame();

    // First we unhighlight the highlighted AYA
    frame->findFirstElement("span.highlighted").removeClass("highlighted");

    // Since each AYA has it own uniq id, we can highlight any AYA in the current page by adding the class "highlighted"
    frame->findFirstElement(QString("span#s%1a%2").arg(pSoraNumber).arg(pAyaNumber)).addClass("highlighted");

    // Get the postion of the selected AYA
    QRect highElement = frame->findFirstElement("span.highlighted").geometry();
    unsigned int ayaPosition = (highElement.y() - (frame->geometry().height() / 2)) + (highElement.height() / 2);

    // Animation the scrolling to the selected AYA
    QPropertyAnimation *animation = new QPropertyAnimation(frame, "scrollPosition");
    animation->setDuration(1000);
    animation->setStartValue(frame->scrollPosition());
    animation->setEndValue(QPoint(0, ayaPosition));

    animation->start();
}

void MainWindow::selectResult(int pSoraNumber, int pAyaNumber)
{
    int pageNumber = this->getAyaPageNumber(pSoraNumber, pAyaNumber);

    this->selectSora(pSoraNumber, pAyaNumber, false);
    this->display(pageNumber, pSoraNumber, pAyaNumber);

}

void MainWindow::setSelectedSora(int pSoraNumber)
{
    QItemSelectionModel *selection = ui->listView->selectionModel();
    QModelIndex itemToSelect = m_sowarNamesModel->index(pSoraNumber - 1, 0, QModelIndex());
    selection->select(itemToSelect, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    ui->listView->scrollTo(itemToSelect);

}

void MainWindow::reloadSoraInfo()
{
    setSoraDetials();
}

void MainWindow::openSora()
{
    openSora(ui->listView->currentIndex());
}

void MainWindow::openSoraInNewTab()
{
    m_tab->addNewOnglet();
    openSora();
}

void MainWindow::addNewTab()
{
    m_tab->addNewOnglet();
    this->selectSora(1);
}

