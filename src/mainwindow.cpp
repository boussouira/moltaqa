#include "mainwindow.h"

#include <QtWebKit>
#include <QSettings>
#include <QStringListModel>
#include <QMessageBox>
#include <QComboBox>
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
    this->setWindowTitle(APP_NAME);
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

    QComboBox *comboTafasir = new QComboBox(this);
    QAction *openSelectedTafsir =  new QAction(QIcon(":/menu/images/arrow-left.png"), OPENSORA, this);
    ui->toolBarTafesir->addWidget(comboTafasir);
    ui->toolBarTafesir->addAction(openSelectedTafsir);

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

void MainWindow::selectSora(int pSoraNumber, int pAyaNumber)
{
    m_quranModel->getSoraInfo(pSoraNumber, pAyaNumber, m_tab->currentSoraInfo());
    this->display(m_tab->currentSoraInfo());
    // Update navigation *buttons*
    updateNavigationActions();
}

void MainWindow::setSoraDetials(SoraInfo *pSoraInfo)
{
    ignoreSignals = true;

    ui->spinBoxAyaNumber->setMaximum(pSoraInfo->ayatCount());
    ui->spinBoxAyaNumber->setSuffix(QString(" / %1").arg(pSoraInfo->ayatCount()));
    ui->spinBoxAyaNumber->setValue(pSoraInfo->currentAya());

    this->setSelectedSora(pSoraInfo->number());
    ignoreSignals = false;
}

void MainWindow::display(SoraInfo *pSoraInfo)
{
    bool emptyPage = m_tab->currentPage()->page()->mainFrame()->toPlainText().isEmpty();

    if(emptyPage or pSoraInfo->currentPage() != ui->spinBoxPageNumber->value())
        m_tab->currentPage()->page()->mainFrame()->setHtml(m_quranModel->getQuranPage(pSoraInfo));

    m_tab->setTabText(m_tab->currentIndex(), QString("%1 %2").arg(SORAT).arg(pSoraInfo->name()));
    this->scrollToAya(pSoraInfo->number(), pSoraInfo->currentAya());
    this->setSoraDetials(m_tab->currentSoraInfo());
    ui->spinBoxAyaNumber->setValue(pSoraInfo->currentAya());
    ui->spinBoxPageNumber->setValue(pSoraInfo->currentPage());
}

void MainWindow::ayaNumberChange(int pNewAyaNumber)
{
    if (m_tab->currentSoraInfo()->currentAya() == pNewAyaNumber or ignoreSignals)
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
    if(!ignoreSignals)
        this->selectSora(pSelection.row()+1);
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this, APP_NAME, ABOUT_APP);
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
    ignoreSignals = false;
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

void MainWindow::on_actionNextAYA_triggered()
{
    int newAyaNumber = ui->spinBoxAyaNumber->value()+1;
    if(newAyaNumber > ui->spinBoxAyaNumber->maximum()) {
        // Goto next SORA
        int nextSora = m_tab->currentSoraInfo()->number()+1;
        // SORA number must be less than 114
        if (nextSora >= 115) {
            // If it's less greater 114 we go to the first SORA in the Quran
            nextSora = 1;
        }
        // Then we select it
        this->selectSora(nextSora);
    } else {
        ui->spinBoxAyaNumber->setValue(newAyaNumber);
    }
}

void MainWindow::on_actionPrevAYA_triggered()
{
    int newAyaNumber = ui->spinBoxAyaNumber->value()-1;
    if(newAyaNumber < ui->spinBoxAyaNumber->minimum()) {
        // Goto previous SORA
        int prevSora = m_tab->currentSoraInfo()->number()-1;
        // We make sure that our SORA number is greater than 0
        if (prevSora <= 0) {
            // less than 0, go to the last SORA
            prevSora = 114;
        }
        this->selectSora(prevSora);
    } else {
        ui->spinBoxAyaNumber->setValue(newAyaNumber);
    }
}

void MainWindow::on_actionNextPage_triggered()
{
    m_quranModel->getSoraInfoByPage(ui->spinBoxPageNumber->value()+1,
                                    m_tab->currentSoraInfo());

    this->selectSora(m_tab->currentSoraInfo()->number(),
                     m_tab->currentSoraInfo()->currentAya());
}

void MainWindow::on_actionPrevPage_triggered()
{
    m_quranModel->getSoraInfoByPage(ui->spinBoxPageNumber->value()-1,
                                    m_tab->currentSoraInfo());

    this->selectSora(m_tab->currentSoraInfo()->number(),
                     m_tab->currentSoraInfo()->currentAya());
}

void MainWindow::updateNavigationActions()
{
    // Page navigation
    if(ui->spinBoxPageNumber->value() >= 604)
        ui->actionNextPage->setEnabled(false);
    else
        ui->actionNextPage->setEnabled(true);

    if(ui->spinBoxPageNumber->value() <= 1)
        ui->actionPrevPage->setEnabled(false);
    else
        ui->actionPrevPage->setEnabled(true);
}
