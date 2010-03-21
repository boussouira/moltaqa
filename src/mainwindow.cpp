#include "constant.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "ksetting.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    
    ui->setupUi(this);
    this->setWindowTitle(APP_NAME);

    QHBoxLayout *layout = new QHBoxLayout;
    m_booksViewr = new BooksViewer(this);
    layout->addWidget(m_booksViewr);
    ui->centralWidget->setLayout(layout);

    this->setupActions();
    m_booksViewr->openSoraInNewTab(1);
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(aboutAlKotobiya()));
    connect(ui->actionNewTab, SIGNAL(triggered()),
            m_booksViewr, SLOT(openSoraInNewTab()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            this, SLOT(settingDialog()));
    connect(m_booksViewr, SIGNAL(updateNavigationButtons()),
            this, SLOT(updateNavigationActions()));

    connect(ui->actionNextAYA, SIGNAL(triggered()),
            m_booksViewr, SLOT(nextAya()));
    connect(ui->actionPrevAYA, SIGNAL(triggered()),
            m_booksViewr, SLOT(previousAYA()));
    connect(ui->actionNextPage, SIGNAL(triggered()),
            m_booksViewr, SLOT(nextPage()));
    connect(ui->actionPrevPage, SIGNAL(triggered()),
            m_booksViewr, SLOT(previousPage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this, APP_NAME, ABOUT_APP);
}

void MainWindow::settingDialog()
{
    m_ksetting = new KSetting(this);
    m_ksetting->exec();
    delete m_ksetting;
}

void MainWindow::updateNavigationActions()
{
    // Page navigation
    ui->actionNextPage->setEnabled(m_booksViewr->showNextPageButton());
    ui->actionPrevPage->setEnabled(m_booksViewr->showPrevPageButton());
}

