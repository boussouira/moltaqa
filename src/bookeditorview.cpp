#include "bookeditorview.h"
#include "ui_bookeditorview.h"
#include <qicon.h>
#include <qaction.h>
#include <QHBoxLayout>
#include <QDebug>
#include "editwebview.h"
#include "utils.h"
#include "stringutils.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "bookpage.h"
#include "openpagedialog.h"
#include "bookexception.h"
#include "bookindexeditor.h"
#include "indextracker.h"

#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qsplitter.h>
#include <qtimer.h>

BookEditorView::BookEditorView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::BookEditorView),
    m_splitter(0),
    m_bookReader(0),
    m_currentPage(0),
    m_indexEdited(false)
{
    ui->setupUi(this);

    m_bookEditor = new BookEditor(this);

    setupView();
    setupToolBar();
}

BookEditorView::~BookEditorView()
{
    Utils::Settings::set("BookEditorView/splitter", m_splitter->saveState());

    ml_delete_check(m_bookReader);

    delete ui;
}

QString BookEditorView::title()
{
    return tr("تحرير الكتب");
}

void BookEditorView::setupView()
{
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setDocumentMode(true);

    m_indexEditor = new BookIndexEditor(this);
    m_webView = new EditWebView(this);
    m_webView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    m_webView->setMinimumWidth(200);

    m_splitter = new QSplitter(this);
    m_splitter->addWidget(m_indexEditor);
    m_splitter->addWidget(m_webView);

    m_splitter->setCollapsible(0, true);
    m_splitter->setCollapsible(1, false);

    ui->tabWidget->addTab(m_splitter, QString());

    if(Utils::Settings::contains("BookEditorView/splitter"))
        m_splitter->restoreState(Utils::Settings::get("BookEditorView/splitter").toByteArray());

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(m_timer, SIGNAL(timeout()), SLOT(checkPageModified()));
    connect(m_indexEditor, SIGNAL(indexEdited()), SLOT(indexChanged()));
}

void BookEditorView::editBook(LibraryBookPtr book, int pageID)
{
    ml_return_on_fail2(book, "BookEditorView::editBook book is null");

    ml_return_on_fail(maySave());

    if(book->isQuran())
        throw BookException(tr("لا يمكن تحرير القرآن الكريم"));

    ml_delete_check(m_bookReader);

    if(book->isNormal()) {
        RichSimpleBookReader *simpleReader = new RichSimpleBookReader();
        simpleReader->setShowShorooh(false);
        m_bookReader = simpleReader;
    } else if(book->isTafessir()) {
        RichTafessirReader *tafessir = new RichTafessirReader();
        tafessir->setShowQuranText(false);
        m_bookReader = tafessir;
    } else {
        throw BookException(QString("Unknow book type %1").arg(book->type), book->path);
    }

    ui->widgetQuran->setVisible(!book->isNormal());

    m_bookReader->setBookInfo(book);
    m_bookReader->openBook();

    m_indexEditor->setup();

    connect(m_bookReader, SIGNAL(textChanged()), SLOT(readerTextChange()));

    m_bookReader->goToPage(pageID);

    ui->tabWidget->setTabText(0, Utils::String::abbreviate(book->title, 40));
    ui->tabWidget->setTabToolTip(0, book->title);

    m_bookEditor->setBookReader(m_bookReader);

    m_indexEdited = false;
    m_timer->start();

    emit showMe();
}

void BookEditorView::setupToolBar()
{
    QToolBar *bar = new QToolBar(tr("تحرير الكتاب"), this);

    QIcon nextIcon  = QIcon::fromTheme("go-previous", QIcon(":/images/go-previous.png"));
    QIcon prevIcon  = QIcon::fromTheme("go-next", QIcon(":/images/go-next.png"));
    QIcon firstIcon = QIcon::fromTheme("go-last", QIcon(":/images/go-last.png"));
    QIcon lastIcon  = QIcon::fromTheme("go-first", QIcon(":/images/go-first.png"));
    QIcon gotoIcon  = QIcon::fromTheme("go-jump", QIcon(":/images/go-jump.png"));

    m_actionSave = bar->addAction(QIcon::fromTheme("document-save", QIcon(":/images/document-save.png")),
                                  tr("حفظ التغييرات"), this, SLOT(save()));
    bar->addSeparator();
    m_actionFirstPage = bar->addAction(firstIcon, tr("الصفحة الاولى"), this, SLOT(firstPage()));
    m_actionPrevPage = bar->addAction(prevIcon, tr("الصفحة السابقة"), this, SLOT(prevPage()));
    m_actionNextPage = bar->addAction(nextIcon, tr("الصفحة التالية"), this, SLOT(nextPage()));
    m_actionLastPage = bar->addAction(lastIcon, tr("الصفحة الاخيرة"), this, SLOT(lastPage()));
    m_actionGotToPage = bar->addAction(gotoIcon, tr("انتقل الى..."), this, SLOT(gotoPage()));
    bar->addSeparator();
    m_actionAddPage = bar->addAction(QIcon(":/images/add.png"), tr("اضافة صفحة"), this, SLOT(addPage()));
    m_actionRemovePage = bar->addAction(QIcon(":/images/remove.png"), tr("حذف الصفحة"), this, SLOT(removePage()));

    m_toolBars << bar;
}

void BookEditorView::updateActions()
{
    if(m_bookReader) {
        m_actionNextPage->setEnabled(m_bookReader->hasNext());
        m_actionLastPage->setEnabled(m_bookReader->hasNext());

        m_actionPrevPage->setEnabled(m_bookReader->hasPrev());
        m_actionFirstPage->setEnabled(m_bookReader->hasPrev());
    }

    m_actionSave->setEnabled(m_pages.size() || m_indexEdited);

    // If we have some saved pages then 'Save' action will be always enabled
    // m_timer is no more needed
    if(m_pages.size() || m_indexEdited)
        m_timer->stop();
}

bool BookEditorView::pageEdited()
{
    return m_currentPage
            ? (m_webView->editorText() != m_currentPage->text
            || ui->spinPage->value() != m_currentPage->page
            || ui->spinPart->value() != m_currentPage->part
            || ui->spinHaddit->value() != m_currentPage->haddit
            || ui->spinSora->value() != m_currentPage->sora
            || ui->spinAya->value() != m_currentPage->aya)
            : false;
}

void BookEditorView::saveCurrentPage()
{
    if(pageEdited()) {
        m_currentPage->page = ui->spinPage->value();
        m_currentPage->part = ui->spinPart->value();

        m_currentPage->haddit = ui->spinHaddit->value();

        m_currentPage->sora = ui->spinSora->value();
        m_currentPage->aya = ui->spinAya->value();

        m_currentPage->text = m_webView->editorText();

        if(!m_pages.value(m_currentPage->pageID, 0))
            m_pages.insert(m_currentPage->pageID, m_currentPage);
    }
}

void BookEditorView::clearChanges()
{
    qDeleteAll(m_pages);
    m_pages.clear();

    m_currentPage = 0;
    m_indexEdited = false;
    m_timer->start();
}

bool BookEditorView::maySave(bool canCancel)
{
    saveCurrentPage();
    updateActions();

    if(m_pages.size() || m_indexEdited) {
        int rep = QMessageBox::question(this,
                                        tr("حفظ التعديلات"),
                                        tr("هل تريد حفظ التعديلات التي اجريتها على كتاب:\n%1؟").arg(m_bookReader->bookInfo()->title),
                                        QMessageBox::Yes|(canCancel ? QMessageBox::No|QMessageBox::Cancel : QMessageBox::No),
                                        canCancel ? QMessageBox::Cancel : QMessageBox::No);
        if(rep == QMessageBox::No) {
            clearChanges();
        } else if(rep == QMessageBox::Yes) {
            save();
        } else {
            return false;
        }
    }

    return true;
}

void BookEditorView::closeBook(bool hide)
{
    clearChanges();

    ml_delete_check(m_bookReader);

    if(hide) {
        ui->tabWidget->setTabText(0, QString());
        ui->tabWidget->setTabToolTip(0, QString());

        emit hideMe();
    }
}

void BookEditorView::setCurrentPage(BookPage *page)
{
    ui->spinPage->setValue(page->page);
    ui->spinPart->setValue(page->part);

    ui->spinAya->setValue(page->aya);
    ui->spinSora->setValue(page->sora);

    if(page->haddit)
        ui->spinHaddit->setValue(page->haddit);

    m_webView->setEditorText(page->text);
}

void BookEditorView::readerTextChange()
{
    saveCurrentPage();

    BookPage *saved = m_pages.value(m_bookReader->page()->pageID, 0);
    m_currentPage = saved ? saved : m_bookReader->page()->clone();

    setCurrentPage(m_currentPage);
    updateActions();
}

void BookEditorView::checkPageModified()
{
    bool pageModified = m_webView->pageModified();
    m_actionSave->setEnabled(m_pages.size() || pageModified || m_indexEdited);
}

void BookEditorView::indexChanged()
{
    m_indexEdited = true;
    checkPageModified();
}

void BookEditorView::save()
{
    saveCurrentPage();

    if(m_pages.size() || m_indexEdited) {
        QProgressDialog dialog(this);
        dialog.setWindowTitle(tr("حفظ التغييرات"));
        dialog.setLabelText(tr("جاري حفظ التغييرات..."));
        dialog.setMaximum(4);
        dialog.setMinimumDuration(0);
        dialog.setValue(0);
        dialog.show();

        m_bookEditor->unZip();
        dialog.setValue(dialog.value()+1);

        if(m_bookEditor->saveBookPages(m_pages.values()))
            clearChanges();

        m_indexEditor->save(m_bookEditor->titlesFile());

        dialog.setValue(dialog.value()+1);

        m_bookEditor->zip();
        dialog.setValue(dialog.value()+1);

        LibraryBookPtr book = m_bookReader->bookInfo();

        if(m_bookEditor->save())
            IndexTracker::instance()->addTask(book->id, IndexTask::Update, false);

        dialog.setValue(dialog.value()+1);

        m_webView->resetUndo();
        m_bookReader->goToPage(m_bookReader->page()->pageID);

        m_timer->start();
    }

    updateActions();
}

void BookEditorView::cancel()
{
    int rep = QMessageBox::question(this,
                                    tr("إلغاء التعديلات"),
                                    tr("هل تريد إلغاء كل التعديلات ؟"),
                                    QMessageBox::Yes|QMessageBox::No,
                                    QMessageBox::No);
    if(rep == QMessageBox::Yes) {
        clearChanges();
        updateActions();
    }
}

void BookEditorView::addPage()
{
    int pageID = m_bookEditor->maxPageID()+1;
    BookPage *page = m_currentPage->clone();
    page->pageID = pageID;
    page->text.clear();

    m_pages.insert(pageID, page);

    m_bookEditor->addPage(pageID);
}

void BookEditorView::removePage()
{
    BookPage *page = m_currentPage->clone();
    m_pages.insert(page->pageID, page);

    m_bookEditor->removePage();
}

void BookEditorView::closeTab(int)
{
    if(maySave()) {
        closeBook();
    }
}

void BookEditorView::nextPage()
{
    if(m_bookReader)
        m_bookReader->nextPage();
}

void BookEditorView::prevPage()
{
    if(m_bookReader)
        m_bookReader->prevPage();
}

void BookEditorView::firstPage()
{
    if(m_bookReader)
        m_bookReader->firstPage();
}

void BookEditorView::lastPage()
{
    if(m_bookReader)
        m_bookReader->lastPage();
}

void BookEditorView::gotoPage()
{
    if(m_bookReader) {
        OpenPageDialog dialog(this);
        dialog.setInfo(m_bookReader->bookInfo(), m_bookReader->page());

        if(dialog.exec() == QDialog::Accepted) {
            if(dialog.currentPage() == 0) // Open selected page/part
                m_bookReader->goToPage(dialog.selectedPage(), dialog.selectedPart());
            else if(dialog.currentPage() == 1) // Open selected sora/page
                m_bookReader->goToSora(dialog.selectedSora(), dialog.selectedAya());
            else if(dialog.currentPage() == 2) // Open selected haddit
                m_bookReader->goToHaddit(dialog.selectedHaddit());
            else
                qDebug("BookEditorView::gotoPage unknow page index %d", dialog.currentPage());
        }
    }
}
