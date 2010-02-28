#include "mainwindow.h"

#include <QtWebKit>
#include <QSettings>
#include <QStringListModel>
#include <QMessageBox>
#include <QDebug>

#include "constant.h"
#include "quransearch.h"
#include "sorainfo.h"
#include "ktab.h"
#include "quranmodel.h"
#include "ksetting.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    
    ui->setupUi(this);
    m_ksetting = new KSetting(this);
    m_tab = new KTab(ui->centralWidget);

    this->loadSettings();
    m_quranModel = new QuranModel(this, m_databasePATH);
    m_search = new QuranSearch(this, m_databasePATH);
    ui->verticalLayout_4->addWidget(m_tab);
    ui->verticalLayout_5->addWidget(m_search);

    m_tab->addNewOnglet();

    this->setupActions();
    this->setupQuranIndex();
    this->setupConnections();
    ui->dockSearch->setShown(false);


}

void MainWindow::setupActions()
{
    QAction *actionOpenSora = new QAction(OPENSORA, ui->listView);
    QAction *actionOpenSoraInNewTab = new QAction(OPENSORAINTAB, this);
    ui->listView->addAction(actionOpenSora);
    ui->listView->addAction(actionOpenSoraInNewTab);

    connect(actionOpenSora, SIGNAL(triggered()), this, SLOT(openSelectedSora()));
    connect(actionOpenSoraInNewTab, SIGNAL(triggered()), this, SLOT(openSelectedSoraInNewTab()));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAlKotobiya()));
    connect(ui->actionNewTab, SIGNAL(triggered()), this, SLOT(addNewTab()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingDialog()));
}

void MainWindow::setupConnections()
{
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openSelectedSora(QModelIndex)));
    connect(ui->spinBoxAyaNumber, SIGNAL(valueChanged(int)), this, SLOT(ayaNumberChange(int)));
    connect(m_search, SIGNAL(resultSelected(int,int)), this, SLOT(selectSora(int,int)));
    connect(ui->dockIndexs, SIGNAL(visibilityChanged(bool)), this, SLOT(hideDockIndex()));
    connect(ui->dockSearch, SIGNAL(visibilityChanged(bool)), this, SLOT(hideDockSearch()));
    connect(ui->actionIndexDock, SIGNAL(toggled(bool)), ui->dockIndexs, SLOT(setShown(bool)));
    connect(ui->actionSearchDock, SIGNAL(toggled(bool)), ui->dockSearch, SLOT(setShown(bool)));
    connect(m_tab, SIGNAL(currentChanged(int)), this, SLOT(reloadSoraInfo()));
    connect(m_tab, SIGNAL(tabCloseRequested(int)), m_tab, SLOT(closeTab(int)));
    connect(m_tab, SIGNAL(reloadCurrentSoraInfo()), this, SLOT(reloadSoraInfo()));
}

void MainWindow::setupQuranIndex()
{
    m_sowarNamesModel = new QStringListModel();
    m_quranModel->getSowarList(m_sowarNamesModel);
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
    m_quranModel->getSoraInfo(pSoraNumber, pAyaNumber, m_tab->currentSoraInfo());
    if(pDisplay)
        this->display(m_tab->currentSoraInfo());
}

void MainWindow::setSoraDetials(SoraInfo *pSoraInfo)
{
    freez = true;

    // Set AYAT count and current AYA number in the spin box
    ui->spinBoxAyaNumber->setMaximum(pSoraInfo->ayatCount());
    ui->spinBoxAyaNumber->setSuffix(QString(" / %1").arg(pSoraInfo->ayatCount()));
    ui->spinBoxAyaNumber->setValue(pSoraInfo->currentAya());

    this->setSelectedSora(pSoraInfo->number());
    freez = false;
}

void MainWindow::display(SoraInfo *pSoraInfo)
{
    if(pSoraInfo->currentPage() != ui->spinBoxPageNumber->value())
        m_tab->currentPage()->page()->mainFrame()->setHtml(m_quranModel->getQuranPage(pSoraInfo));
    m_tab->setTabText(m_tab->currentIndex(), QString("%1 %2").arg(SORAT).arg(pSoraInfo->name()));
    this->scrollToAya(pSoraInfo->number(), pSoraInfo->currentAya());
    this->setSoraDetials(m_tab->currentSoraInfo());
    ui->spinBoxAyaNumber->setValue(pSoraInfo->currentAya());
    ui->spinBoxPageNumber->setValue(pSoraInfo->currentPage());
}

void MainWindow::ayaNumberChange(int pNewAyaNumber)
{
    // If freez == true then break.
    if (m_tab->currentSoraInfo()->currentAya() == pNewAyaNumber or freez)
        return ;
    int page = m_quranModel->getAyaPageNumber(m_tab->currentSoraInfo()->number(), pNewAyaNumber);
    if(page != m_tab->currentSoraInfo()->currentPage()) {
        m_tab->currentSoraInfo()->setCurrentPage(page);
    }
    m_tab->currentSoraInfo()->setCurrentAya(pNewAyaNumber);
    this->display(m_tab->currentSoraInfo());
}

void MainWindow::openSelectedSora()
{
    openSelectedSora(ui->listView->currentIndex());
}

void MainWindow::openSelectedSora(QModelIndex pSelection)
{
    if(!freez)
        this->selectSora(pSelection.row()+1);
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this, "About", "Al Kotobiya");
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_databasePATH = settings.value("app/db").toString();
    if(!QFile::exists(m_databasePATH)) {
        int rep = m_ksetting->exec();
        if(rep == 0) {
            qDebug("Error: No database selected");
            exit(1);
        } else {
            m_databasePATH = settings.value("app/db").toString();
        }
    }
    freez = false;
}

void MainWindow::settingDialog()
{
    m_ksetting->exec();
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
    // Frame heihgt
    int frameHeihgt = frame->geometry().height() / 2;
    // The height that should be added to center the selected aya
    int addHeight = highElement.height() / 2 ;
    // it must be less than frameHeight
    while (frameHeihgt < addHeight )
        addHeight = addHeight / 2;
    // The aya position equal ((ayaHeight - frameHeight) + addHeight)
    unsigned int ayaPosition = (highElement.y() - frameHeihgt) + addHeight;

    // Animation the scrolling to the selected AYA
    QPropertyAnimation *animation = new QPropertyAnimation(frame, "scrollPosition");
    animation->setDuration(1000);
    animation->setStartValue(frame->scrollPosition());
    animation->setEndValue(QPoint(0, ayaPosition));

    animation->start();
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
    setSoraDetials(m_tab->currentSoraInfo());
}

void MainWindow::openSelectedSoraInNewTab()
{
    m_tab->addNewOnglet();
    openSelectedSora();
}

void MainWindow::addNewTab()
{
    m_tab->addNewOnglet();
    this->selectSora(1);
}

