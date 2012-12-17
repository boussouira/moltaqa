#include "exportdialogpages.h"
#include "filechooserwidget.h"
#include "librarymanager.h"
#include "booklistmanager.h"
#include "filterlineedit.h"
#include "modelutils.h"
#include "modelviewfilter.h"
#include "librarysearchfilter.h"
#include "utils.h"
#include "exportdialog.h"
#include "exportformats.h"

#include <qboxlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdebug.h>
#include <qtreeview.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qdesktopservices.h>
#include <qurl.h>
#include <qscrollbar.h>

/* IntroPage class */

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(tr("تصدير الكتب"));
    //page->setSubTitle(tr("من فضلك قم باختيار صيغة التصدير ومجلد وضع الملفات."));

    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *label = new QLabel(tr("من فضلك قم باختيار صيغة التصدير ومجلد وضع الملفات."));
    label->setWordWrap(true);
    layout->addWidget(label);

    QComboBox *combo = new QComboBox(this);
    combo->insertItem(MOLTAQA_FROMAT, tr("مكتبة الملتقى (mlp)"));
    combo->insertItem(EPUB_FROMAT, tr("كتاب الكتروني (epub)"));
    combo->insertItem(HTML_FORMAT, tr("وويب (html)"));
    //combo->insertItem(PDF_FROMAT, "PDF");

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("صيغة التصدير:")));
    hbox->addWidget(combo);
    hbox->addStretch();
    layout->addLayout(hbox);

    FileChooserWidget *chooser = new FileChooserWidget;
    chooser->setSettingName("ExportDialog");
    layout->addWidget(chooser);

    layout->addStretch();

    m_checkExportInOnePackage = new QCheckBox(tr("تصدير كل الكتب في ملف واحد"), this);
    m_checkExportInOnePackage->setChecked(Utils::Settings::get("ExportDialog/exportInOnePackage", false).toBool());
    m_checkRemoveTashkil = new QCheckBox(tr("حذف تشكيل النص"), this);
    m_checkAddPageNumber = new QCheckBox(tr("ذكر الجزء والصفحة عقب كل صفحة"), this);

    layout->addWidget(m_checkExportInOnePackage);
    layout->addWidget(m_checkRemoveTashkil);
    layout->addWidget(m_checkAddPageNumber);

    setLayout(layout);

    registerField("export.type", combo);
    registerField("export.outdir", chooser->lineEdit());

    registerField("export.ExportInOnePackage", m_checkExportInOnePackage);
    registerField("export.removeTashkil", m_checkRemoveTashkil);
    registerField("export.addPageNumber", m_checkAddPageNumber);

    connect(combo, SIGNAL(currentIndexChanged(int)), SLOT(formatChanged(int)));
    formatChanged(MOLTAQA_FROMAT);
}

bool IntroPage::validatePage()
{
    if(field("export.outdir").toString().trimmed().isEmpty()) {
        QMessageBox::warning(this,
                             tr("تصدير الكتب"),
                             tr("لم تقم بإختيار مجلد وضع الكتب"));
        return false;
    }

    if(m_checkExportInOnePackage->isVisible()) {
        Utils::Settings::set("ExportDialog/exportInOnePackage",
                             m_checkExportInOnePackage->isChecked());
    }

    return true;
}

void IntroPage::formatChanged(int index)
{
    switch (index) {
    case MOLTAQA_FROMAT:
        m_checkExportInOnePackage->setVisible(true);
        m_checkAddPageNumber->setVisible(false);
        m_checkRemoveTashkil->setVisible(false);
        break;
    case PDF_FROMAT:
        m_checkExportInOnePackage->setVisible(false);
        m_checkAddPageNumber->setVisible(false);
        m_checkRemoveTashkil->setVisible(false);
        break;
    case EPUB_FROMAT:
        m_checkExportInOnePackage->setVisible(false);
        m_checkAddPageNumber->setVisible(true);
        m_checkRemoveTashkil->setVisible(true);
        break;
    case HTML_FORMAT:
        m_checkExportInOnePackage->setVisible(false);
        m_checkAddPageNumber->setVisible(true);
        m_checkRemoveTashkil->setVisible(true);
        break;
    default:
        break;
    }
}

/* BookSelectionPage class */

BookSelectionPage::BookSelectionPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(tr("اختيار الكتب"));

    QVBoxLayout *layout = new QVBoxLayout;

    FilterLineEdit *line = new FilterLineEdit(this);
    layout->addWidget(line);

    QStandardItemModel *m_booksModel = LibraryManager::instance()->bookListManager()->bookListModel();
    m_booksModel->setColumnCount(2);
    Utils::Model::setModelCheckable(m_booksModel);

    QTreeView *treeView = new QTreeView(this);

    m_filter = new LibrarySearchFilter(this);
    m_filter->setTreeView(treeView);
    m_filter->setLineEdit(line);
    m_filter->loadModel();

    treeView->setModel(m_filter->filterModel());
    treeView->setColumnWidth(0, 300);

    Utils::Widget::restore(treeView, "ExportDialog",
                           QList<int>() << 350 << 200);

    layout->addWidget(treeView);

    setLayout(layout);
}

bool BookSelectionPage::validatePage()
{
        m_filter->generateLists();
        if(!m_filter->selectedCount()) {
            QMessageBox::warning(this,
                                 tr("اختيار الكتب"),
                                 tr("لم تقم بإختيار أي كتاب"));
            return false;
        }

        ExportDialog *w = qobject_cast<ExportDialog*>(wizard());
        ml_return_val_on_fail2(w, "BookSelectionPage::validatePage Can't get wizard", false);

        w->setBooksToImport(m_filter->selectedID());

        return true;
}

/* ExportPage class */

ExportPage::ExportPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(tr("بدأ التصدير"));

    QVBoxLayout *layout = new QVBoxLayout;

    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    layout->addWidget(m_label);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setFormat("%p% (%v/%m)");
    m_progressBar->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_progressBar);

    m_treeWidget = new QListWidget(this);
    layout->addWidget(m_treeWidget);

    m_checkOpenOutFolder = new QCheckBox(tr("فتح مجلد وضع الملفات عند الانتهاء من التصدير"), this);
    m_checkOpenOutFolder->setChecked(Utils::Settings::get("ExportDialog/openOutDit", true).toBool());
    layout->addWidget(m_checkOpenOutFolder);

    setLayout(layout);

    m_doneExport = false;
    m_thread = new BookExporterThread(this);

    connect(m_thread, SIGNAL(bookExported(QString)), SLOT(bookExported(QString)));
    connect(m_thread, SIGNAL(doneExporting()), SLOT(doneExporting()));
}

void ExportPage::initializePage()
{
    ExportDialog *w = qobject_cast<ExportDialog*>(wizard());
    ml_return_on_fail2(w, "ExportPage::initializePage Can't get wizard");

    QList<int> books = w->booksToImport();
    m_label->setText(tr("جاري تصدير الكتب..."));

    ExportFormat format = static_cast<ExportFormat>(field("export.type").toInt());
    QString outDir = field("export.outdir").toString();

    m_progressBar->setMaximum(books.count());

    m_thread->setExportFormat(format);
    m_thread->setExportInOnePackage(field("export.ExportInOnePackage").toBool());
    m_thread->setRemoveTashkil(field("export.removeTashkil").toBool());
    m_thread->setAddPageNumber(field("export.addPageNumber").toBool());

    m_thread->setBooksToExport(books);
    m_thread->setOutDir(outDir);

    m_thread->start();
}

bool ExportPage::isComplete() const
{
    return m_doneExport;
}

void ExportPage::openOutDir()
{
    if(m_checkOpenOutFolder->isChecked()) {
        QString outDir = field("export.outdir").toString();
        QDesktopServices::openUrl(QUrl(outDir));
    }

    Utils::Settings::set("ExportDialog/openOutDit", m_checkOpenOutFolder->isChecked());
}

void ExportPage::bookExported(QString book)
{

    bool scrollToBottom = (m_treeWidget->verticalScrollBar()->maximum()
                           == m_treeWidget->verticalScrollBar()->value());

    m_treeWidget->addItem(new QListWidgetItem(QIcon(":/images/add2.png"), book));
    m_progressBar->setValue(m_progressBar->value()+1);

    if(scrollToBottom)
        m_treeWidget->scrollToBottom();
}

void ExportPage::doneExporting()
{
    m_progressBar->setValue(m_progressBar->maximum());
    m_doneExport = true;

    emit completeChanged();
}
