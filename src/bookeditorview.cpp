#include "bookeditorview.h"
#include "ui_bookeditorview.h"
#include <qicon.h>
#include <qaction.h>
#include <QHBoxLayout>
#include <QDebug>
#include "editwebview.h"
#include "utils.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "bookpage.h"
#include "openpagedialog.h"
#include "bookexception.h"
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qmessagebox.h>

BookEditorView::BookEditorView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::BookEditorView),
    m_bookReader(0),
    m_currentPage(0)
{
    ui->setupUi(this);

    setupView();
    setupToolBar();
}

BookEditorView::~BookEditorView()
{
    if(m_bookReader)
        delete m_bookReader;
}

QString BookEditorView::title()
{
    return tr("تحرير الكتب");
}

void BookEditorView::setupView()
{
    ui->tabWidget->setTabsClosable(true);

    QWidget *w = new QWidget(this);
    QVBoxLayout *webLayout = new QVBoxLayout(w);
    m_webView = new EditWebView(this);
    webLayout->addWidget(m_webView);
    w->setLayout(webLayout);

    ui->tabWidget->addTab(w, QString());

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
}

void BookEditorView::editBook(LibraryBook *book, BookPage *page)
{
    Q_CHECK_PTR(book);
    Q_CHECK_PTR(page);

    if(!maySave())
        return;

    if(book->isQuran())
        throw BookException(tr("لا يمكن تحرير القرآن الكريم"));

    if(m_bookReader) {
        delete m_bookReader;
        m_bookReader = 0;
    }

    if(book->isNormal())
        m_bookReader = new RichSimpleBookReader();
    else if(book->isTafessir())
        m_bookReader = new RichTafessirReader(0, false);
    else
        qCritical("editBook: Unknow book type %d", book->bookType);

    m_bookReader->setBookInfo(book);
    m_bookReader->setLibraryManager(MW->libraryManager());
    m_bookReader->openBook();

    connect(m_bookReader, SIGNAL(textChanged()), SLOT(readerTextChange()));

    m_bookReader->goToPage(page->pageID);

    ui->tabWidget->setTabText(0, Utils::abbreviate(book->bookDisplayName, 40));
    ui->tabWidget->setTabToolTip(0, book->bookDisplayName);

    emit showMe();
}

void BookEditorView::setupToolBar()
{
    QToolBar *bar = new QToolBar(this);

    QIcon nextIcon  = QIcon::fromTheme("go-previous", QIcon(":/menu/images/go-previous.png"));
    QIcon prevIcon  = QIcon::fromTheme("go-next", QIcon(":/menu/images/go-next.png"));
    QIcon firstIcon = QIcon::fromTheme("go-last", QIcon(":/menu/images/go-last.png"));
    QIcon lastIcon  = QIcon::fromTheme("go-first", QIcon(":/menu/images/go-first.png"));
    QIcon gotoIcon  = QIcon::fromTheme("go-jump", QIcon(":/menu/images/go-jump.png"));

    m_actionSave = bar->addAction(QIcon::fromTheme("document-save", QIcon(":/menu/images/document-save.png")),
                                  tr("حفظ التغييرات"), this, SLOT(save()));
    m_actionCancel = bar->addAction(QIcon::fromTheme("document-revert", QIcon(":/menu/images/document-revert.png")),
                                    tr("الغاء التغييرات"), this, SLOT(cancel()));
    bar->addSeparator();
    m_actionFirstPage = bar->addAction(firstIcon, tr("الصفحة الاولى"), this, SLOT(firstPage()));
    m_actionPrevPage = bar->addAction(prevIcon, tr("الصفحة السابقة"), this, SLOT(prevPage()));
    m_actionNextPage = bar->addAction(nextIcon, tr("الصفحة التالية"), this, SLOT(nextPage()));
    m_actionLastPage = bar->addAction(lastIcon, tr("الصفحة الاخيرة"), this, SLOT(lastPage()));
    m_actionGotToPage = bar->addAction(gotoIcon, tr("انتقل الى..."), this, SLOT(gotoPage()));
//    bar->addSeparator();
//    m_actionPreview = bar->addAction(QIcon(":/menu/images/page-preview.png"), tr("استعراض الصفحة"), this, SLOT(preview()));

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

    m_actionSave->setEnabled(!m_pages.isEmpty());
    m_actionCancel->setEnabled(!m_pages.isEmpty());
}

void BookEditorView::saveCurrentPage()
{
    if(m_currentPage) {
        QString pageText = m_webView->editorText();

        if(pageText != m_currentPage->text) {
            m_currentPage->text = m_webView->editorText();

            if(!m_pages.value(m_currentPage->pageID, 0))
                m_pages.insert(m_currentPage->pageID, m_currentPage);
        }
    }
}

void BookEditorView::clearChanges()
{
    qDeleteAll(m_pages);
    m_pages.clear();

    m_currentPage = 0;
}

bool BookEditorView::maySave(bool canCancel)
{
    saveCurrentPage();
    updateActions();

    if(!m_pages.isEmpty()) {
        int rep = QMessageBox::question(this,
                                        tr("حفظ التعديلات"),
                                        tr("هل تريد حفظ التعديلات التي اجريتها على كتاب:\n%1؟").arg(m_bookReader->bookInfo()->bookDisplayName),
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

void BookEditorView::closeBook()
{
    clearChanges();

    if(m_bookReader) {
        delete m_bookReader;
        m_bookReader = 0;
    }

    ui->tabWidget->setTabText(0, QString());
    ui->tabWidget->setTabToolTip(0, QString());

    emit hideMe();
}

void BookEditorView::readerTextChange()
{
    saveCurrentPage();

    BookPage *saved = m_pages.value(m_bookReader->page()->pageID, 0);
    m_currentPage = saved ? saved : m_bookReader->page()->clone();

    m_webView->setEditorText(m_currentPage->text);
    updateActions();
}

void BookEditorView::save()
{
    saveCurrentPage();

    if(!m_pages.isEmpty()) {
        if(m_bookReader->saveBookPages(m_pages.values())) {
            clearChanges();
        }
    }

    updateActions();
}

void BookEditorView::cancel()
{
    int rep = QMessageBox::question(this,
                                    tr("الغاء التعديلات"),
                                    tr("هل تريد الغاء كل التعديلات ؟"),
                                    QMessageBox::Yes|QMessageBox::No,
                                    QMessageBox::No);
    if(rep == QMessageBox::Yes) {
        clearChanges();
        updateActions();
    }
}

void BookEditorView::preview()
{
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
        m_bookReader->goToPage(-1);
}

void BookEditorView::lastPage()
{
    if(m_bookReader)
        m_bookReader->goToPage(-2);
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
                qDebug("What to do?");
        }
    }
}
