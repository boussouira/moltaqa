#include "bookeditorview.h"
#include "ui_bookeditorview.h"
#include "bookeditor.h"
#include "bookindexeditor.h"
#include "bookmediaeditor.h"
#include "bookpage.h"
#include "bookreaderhelper.h"
#include "editwebview.h"
#include "indextracker.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "openpagedialog.h"
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "stringutils.h"
#include "utils.h"
#include "webpagenam.h"

#include <qaction.h>
#include <qboxlayout.h>
#include <qicon.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qtoolbar.h>

BookEditorView::BookEditorView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::BookEditorView),
    m_splitter(0),
    m_bookReader(0),
    m_currentPage(0)
{
    ui->setupUi(this);

    m_bookEditor = new BookEditor(this);
    m_mediaEditor = new BookMediaEditor(0);
    m_mediaEditor->setWindowIcon(windowIcon());

    setupView();
    setupToolBar();
}

BookEditorView::~BookEditorView()
{
    Utils::Settings::set("BookEditorView/splitter", m_splitter->saveState());

    ml_delete_check(m_bookReader);
    ml_delete_check(m_mediaEditor);

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
    connect(m_indexEditor, SIGNAL(indexDataChanged()), SLOT(checkPageModified()));
    connect(m_mediaEditor, SIGNAL(insertImage(QString)),
            m_webView, SLOT(insertImage(QString)));
    connect(m_mediaEditor, SIGNAL(insertImage(QString)),
            SLOT(activateWindowSlot()));
}

void BookEditorView::editBook(LibraryBook::Ptr book, int pageID)
{
    ml_return_on_fail2(book, "BookEditorView::editBook book is null");

    ml_return_on_fail(maySave());

    if(book->isQuran())
        throw BookException(tr("لا يمكن تحرير القرآن الكريم"));

    if(!QFileInfo(book->path).isWritable()) {
        throw BookException(tr("ملف الكتاب غير قابل للتعديل، من فضلك شغل البرنامج") + QString(" ") +
                       #ifdef Q_OS_WIN
                               tr("كمسؤول"));
                       #else
                               tr("بالصلاحيات المناسبة"));
                       #endif
    }

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
    m_mediaEditor->setBook(book);

    ui->tabWidget->setTabText(0, Utils::String::abbreviate(book->title, 40));
    ui->tabWidget->setTabToolTip(0, book->title);

    m_bookEditor->setBookReader(m_bookReader);
    m_webView->setBook(book);

    WebPageNAM *nam = m_webView->getPageNAM();
    if(nam)
        nam->setBookMedia(m_mediaEditor);
    else
        qWarning() << "BookEditorView::editBook Can't get WebPageNAM";

    m_timer->start();

    connect(m_bookReader, SIGNAL(textChanged()), SLOT(readerTextChange()));
    m_bookReader->goToPage(pageID);

    emit showMe();
}

void BookEditorView::setupToolBar()
{
    QToolBar *bar = new QToolBar(tr("تحرير الكتاب"), this);

    QIcon nextIcon  = ml_theme_icon("go-previous", ":/images/go-previous.png");
    QIcon prevIcon  = ml_theme_icon("go-next", ":/images/go-next.png");
    QIcon firstIcon = ml_theme_icon("go-last", ":/images/go-last.png");
    QIcon lastIcon  = ml_theme_icon("go-first", ":/images/go-first.png");
    QIcon gotoIcon  = ml_theme_icon("go-jump", ":/images/go-jump.png");

    m_actionSave = bar->addAction(ml_theme_icon("document-save", ":/images/document-save.png"),
                                  tr("حفظ التغييرات"), this, SLOT(save()));
    bar->addSeparator();

    m_actionFirstPage = bar->addAction(firstIcon, tr("الصفحة الاولى"), this, SLOT(firstPage()));
    m_actionPrevPage = bar->addAction(prevIcon, tr("الصفحة السابقة"), this, SLOT(prevPage()));
    m_actionNextPage = bar->addAction(nextIcon, tr("الصفحة التالية"), this, SLOT(nextPage()));
    m_actionLastPage = bar->addAction(lastIcon, tr("الصفحة الاخيرة"), this, SLOT(lastPage()));
    m_actionGotToPage = bar->addAction(gotoIcon, tr("انتقل الى..."), this, SLOT(gotoPage()));
    bar->addSeparator();

    bar->addAction(QIcon(":/images/insert-image.png"), tr("اضافة الصور"), this, SLOT(editImages()));
    bar->addSeparator();

    m_actionAddPage = bar->addAction(QIcon(":/images/add.png"), tr("اضافة صفحة"), this, SLOT(addPage()));
    m_actionRemovePage = bar->addAction(QIcon(":/images/remove.png"), tr("حذف الصفحة"), this, SLOT(removePage()));

    bar->setObjectName("BookEditorView.Edit");
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

    m_actionSave->setEnabled(bookEdited());

    // If we have some saved pages then 'Save' action will be always enabled
    // m_timer is no more needed
    if(m_pages.size() || m_indexEditor->indexEdited())
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
    m_indexEditor->setIndexEdited(false);
    m_timer->start();
}

bool BookEditorView::maySave(bool canCancel)
{
    ml_return_val_on_fail(m_bookReader, true);

    saveCurrentPage();
    updateActions();

    if(bookEdited()) {
        int rep = QMessageBox::question(this,
                                        tr("حفظ التعديلات"),
                                        tr("هل تريد حفظ التعديلات التي اجريتها على كتاب:\n%1؟").arg(m_bookReader->book()->title),
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

bool BookEditorView::bookEdited()
{
    return m_pages.size()
            || m_indexEditor->indexEdited()
            || m_webView->pageModified()
            || m_mediaEditor->mediaEdited();
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
    m_actionSave->setEnabled(bookEdited());
}

void BookEditorView::activateWindowSlot()
{
    if(!isActiveWindow())
        activateWindow();
}

void BookEditorView::save()
{
    saveCurrentPage();

    if(bookEdited()) {
        QProgressDialog dialog(this);
        dialog.setWindowTitle(tr("حفظ التغييرات"));
        dialog.setLabelText(tr("جاري حفظ التغييرات..."));
        dialog.setMaximum(4);
        dialog.setMinimumDuration(0);
        dialog.setValue(0);
        dialog.show();

        qApp->processEvents();

        m_bookEditor->unZip();
        dialog.setValue(dialog.value()+1);

        m_bookEditor->saveBookPages(m_pages.values());

        m_indexEditor->save(m_bookEditor->zipHelper());

        m_mediaEditor->saveChanges(m_bookEditor);

        dialog.setValue(dialog.value()+1);

        if(!m_bookEditor->zip()) {
            dialog.hide();

            QMessageBox::warning(this,
                                 tr("تحرير الكتاب"),
                                 tr("حدث خطأ أثناء حفظ التعديلا"));
            return;
        }

        dialog.setValue(dialog.value()+1);

        LibraryBook::Ptr book = m_bookReader->book();

        if(m_bookEditor->save())
            IndexTracker::instance()->addTask(book->id, IndexTask::Update, false);

        dialog.setValue(dialog.value()+1);

        clearChanges();

        m_webView->resetUndo();
        m_bookReader->goToPage(m_bookReader->page()->pageID);
        MW->readerHelper()->removeBookModel(book->id);

        LibraryManager::instance()->bookManager()->updateBook(book, true);

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
    QMenu menu(this);
    QAction *afterAction = menu.addAction(tr("بعد الصفحة الحالية"));
    QAction *beforeAction = menu.addAction(tr("قبل الصفحة الحالية"));

    bool insertAfter = true;
    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
         if(ret == afterAction)
             insertAfter = true;
         else if(ret == beforeAction)
             insertAfter = false;
    }

    int pageID = m_bookEditor->maxPageID()+1;
    BookPage *page = (m_currentPage ? m_currentPage->clone() : new BookPage);
    page->pageID = pageID;
    page->text.clear();

    m_pages.insert(pageID, page);

    m_bookEditor->addPage(pageID, insertAfter);
}

void BookEditorView::removePage()
{
    ml_return_on_fail(m_currentPage);

    if(m_bookReader->pagesCount()==1) {
        QMessageBox::warning(this,
                             title(),
                             tr("لا يمكن حذف الصفحة الأخيرة من الكتاب"));
        return;
    }

    ml_return_on_fail(QMessageBox::question(this,
                                            tr("حذف الصفحة"),
                                            tr("هل انت متأكد من انك تريد حذف هذه الصفحة؟"),
                                            QMessageBox::Yes|QMessageBox::No,
                                            QMessageBox::No)==QMessageBox::Yes);

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

void BookEditorView::editImages()
{
    m_mediaEditor->setBook(m_bookReader->book());
    m_mediaEditor->show();

    if(!m_mediaEditor->isActiveWindow())
        m_mediaEditor->activateWindow();
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
        dialog.setInfo(m_bookReader->book(), m_bookReader->page());

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
