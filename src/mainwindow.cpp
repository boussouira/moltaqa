#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "bookslistbrowser.h"
#include "settingsdialog.h"
#include "bookwidget.h"
#include "settingschecker.h"
#include "importdialog.h"
#include "bookexception.h"
#include "connectioninfo.h"
#include "sqliteconnection.h"
#include "sqliteindexdb.h"

#include <qmessagebox.h>
#include <qsettings.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("برنامج الكتبية"));
    loadSettings();

    try {
        /* Temporary code */
        QSettings settings;
        settings.beginGroup("General");
        QString booksFolder = settings.value("books_folder").toString();
        QString indexDBName = settings.value("index_db").toString();
        settings.endGroup();

        ConnectionInfo *connection = new SqliteConnection;
        connection->setPath(booksFolder + "/" + indexDBName);
        connection->setBooksDir(booksFolder);
        connection->setConnectionName("Conn_1");
        /* Temporary code */

        if(connection->type() == ConnectionInfo::SQLITE)
            m_indexDB = new SqliteIndexDB(connection);
        else
            throw BookException(tr("لم يمكن تحديد نوع الكتاب"));

        m_indexDB->open();

        m_bookView = new BooksViewer(m_indexDB, this);
        m_booksList = new BooksListBrowser(m_indexDB, this);

        m_createMenu = true;
        m_bookView->hide();

        setupActions();
    } catch(BookException &e) {
        QMessageBox::information(this,
                                 tr("برنامج الكتبية"),
                                 tr("حدث خطأ أثناء تحميل البرنامج:"
                                    "<br>%1").arg(e.what()));
        setEnabled(false);
    }
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAlKotobiya()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingDialog()));

    connect(ui->pushOpenQuran, SIGNAL(clicked()), this, SLOT(quranWindow()));
    connect(ui->pushBooksList, SIGNAL(clicked()), this, SLOT(showBooksList()));
    connect(m_booksList, SIGNAL(bookSelected(int)), this, SLOT(openBook(int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), this, SLOT(showBooksList()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_indexDB;
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this,
                             tr("برنامج الكتبية"),
                             tr("برنامج الكتبية لقراءة القرءان الكريم"));
}

void MainWindow::settingDialog()
{
    SettingsDialog settingDialog(this);
    settingDialog.exec();
}

void MainWindow::quranWindow()
{
    openBook(defaultQuran);
}

void MainWindow::openBook(int pBookID)
{
    if(m_createMenu){
        m_bookView->createMenus(this);
        m_createMenu = false;
    }

    try {
        m_bookView->openBook(pBookID);

        if(m_bookView->isHidden())
            m_bookView->show();

        setCentralWidget(m_bookView);

    } catch(BookException &e) {
        QMessageBox::information(this,
                                 tr("برنامج الكتبية"),
                                 tr("حدث خطأ أثناء فتح الكتاب:"
                                    "<br>%1").arg(e.what()));
    }
}

void MainWindow::showBooksList()
{
    m_booksList->show();
}

void MainWindow::loadSettings()
{
    SettingsChecker checker(this);
    checker.checkSettings();

    QSettings settings;
    defaultQuran = settings.value("Books/default_quran", -1).toInt();
    ui->pushOpenQuran->setEnabled(defaultQuran != -1);
}

void MainWindow::on_actionImportFromShamela_triggered()
{
    ImportDialog *dialog = new ImportDialog(m_indexDB, this);

    connect(dialog, SIGNAL(openBook(int)), this, SLOT(openBook(int)));

    if(dialog->exec() == QDialog::Accepted)
        m_booksList->showBooksList();
}
