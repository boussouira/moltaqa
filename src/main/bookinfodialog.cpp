#include "bookinfodialog.h"
#include "authorsmanager.h"
#include "htmlhelper.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "utils.h"
#include "webview.h"

#include <qboxlayout.h>
#include <qevent.h>

BookInfoDialog::BookInfoDialog(QWidget *parent) : QWidget(parent)
{
    setWindowIcon(MW->windowIcon());
    setAttribute(Qt::WA_DeleteOnClose);

    m_view = new WebView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);

    Utils::Widget::restore(this, "BookInfoDialog");
}

void BookInfoDialog::setLibraryBook(LibraryBook::Ptr book)
{
    m_book = book;
    m_view->setBook(book);
}

void BookInfoDialog::setup()
{
    ml_return_on_fail2(m_book, "BookInfoDialog::setup book is null");

    setWindowTitle(m_book->title);

    LibraryBookMeta::Ptr bookMeta = LibraryManager::instance()->bookManager()->getLibraryBookMeta(m_book->id);

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.setTitle(m_book->title);
    html.addCSS("default.css");
    html.addExtraCss();
    html.endHead();

    html.beginBody();

    html.beginDiv(".rawi-info");

    html.beginDiv("#info");
    html.beginDL(".dl-horizontal");

    html.insertDT(tr("الكتاب:"));
    html.insertDD(m_book->title);

    if(!m_book->isQuran()) {
        QString authorDeath;
        AuthorInfo::Ptr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
        if(author) {
            if(author->isALive)
                authorDeath = tr("(معاصر)");
            else if(!author->unknowDeath)
                authorDeath = tr("(%1)").arg(author->deathStr);
        }

        html.insertDT(tr("المؤلف:"));
        html.beginHtmlTag("dd", ".pro-value");
        html.insertAuthorLink(m_book->authorName + " " + authorDeath, m_book->authorID);
        html.endHtmlTag();
    }

    if(m_book->edition.size()) {
        html.insertDT(tr("الطبعة:"));
        html.insertDD(m_book->edition);
    }

    if(m_book->publisher.size()) {
        html.insertDT(tr("الناشر:"));
        html.insertDD(m_book->publisher);
    }

    if(m_book->mohaqeq.size()) {
        html.insertDT(tr("المحقق:"));
        html.insertDD(m_book->mohaqeq);
    }

    html.insertDT(tr("ترقيم الكتاب:"));

    if(m_book->bookFlags & LibraryBook::PrintedPageNumber)
        html.insertDD(tr("موافق للمطبوع"));
    else if(m_book->bookFlags & LibraryBook::MakhetotPageNumer)
        html.insertDD(tr("موافق للمخطوط"));
    else
        html.insertDD(tr("غير موافق للمطبوع"));

    QString moqabal;
    if(m_book->bookFlags & LibraryBook::MoqabalMoteboa)
        moqabal += tr("المطبوع");
    else if(m_book->bookFlags & LibraryBook::MoqabalMakhetot)
        moqabal += tr("المخطوط");
    else if(m_book->bookFlags & LibraryBook::MoqabalPdf)
        moqabal += tr("نسخة مصورة PDF");

    if(moqabal.size()) {
        html.insertDT(tr("مقابل على:"));
        html.insertDD(moqabal);
    }

    QString otherInfo;
    if(m_book->bookFlags & LibraryBook::LinkedWithShareeh)
        otherInfo += tr("مرتبط بشرح، ");
    if(m_book->bookFlags & LibraryBook::HaveFootNotes)
        otherInfo += tr("مذيل بالحواشي، ");
    if(m_book->bookFlags & LibraryBook::Mashekool)
        otherInfo += tr("مشكول، ");

    if(otherInfo.size()) {
        html.insertDT(tr("معلومات اخرى:"));
        html.insertDD(otherInfo);
    }

    if(m_book->comment.size()) {
        html.insertDT(tr("ملاحظات:"));
        // Copy the object before editing it
        html.insertDD(QString(m_book->comment).replace("\n", "<br>"));
    }

    if(!m_book->isQuran()) {
        try {
            RichBookReader *reader = 0;

            if(m_book->isNormal())
                reader = new RichSimpleBookReader();
            else if(m_book->isTafessir())
                reader = new RichTafessirReader();
            else
                throw BookException(tr("لم يتم التعرف على نوع الكتاب"), QString("Book Type: %1").arg(m_book->type));

            reader->setBookInfo(m_book);
            reader->openBook();

            int pagesCount = reader->pagesCount();
            if(pagesCount) {
                html.insertDT(tr("عدد الصفحات:"));
                html.insertDD(QString::number(pagesCount));
            }

        } catch (BookException &e) {
            e.print();
        }
    }

    if(bookMeta) {
        html.insertDT(tr("تاريخ الاضافة:"));
        html.insertDD(bookMeta->importDateStr());

        html.insertDT(tr("آخر تحديث:"));
        html.insertDD(bookMeta->updateDateStr());

        html.insertDT(tr("عدد مرات تصفح الكتاب:"));
        html.insertDD(QString::number(bookMeta->openCount));

        html.insertDT(tr("عدد مرات تعديل الكتاب:"));
        html.insertDD(QString::number(bookMeta->updateCount));
    }

    html.endDL(); // .dl-horizontal
    html.endDiv(); // #info

    if(m_book->info.size()) {
        html.insertHead(4, tr("نبذة حول الكتاب"));
        html.insertDiv(m_book->info, ".head-info");
    }

    html.insertDiv(tr("معلومات اضافية"), ".book-extra-info");
    html.beginDiv(".book-extra-info-data");

    html.insertParagraph(QString("ID: %1").arg(m_book->id));
    html.insertParagraph(QString("UUID: %1").arg(m_book->uuid));

    if(bookMeta)
        html.insertParagraph(QString("MD5: %1").arg(bookMeta->fileChecksum));

    html.insertParagraph(QString("PATH: %1").arg(m_book->path));

    html.endDiv();

    html.endDiv(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.addJSCode("setupToolTip();");

    html.endAll();

    m_view->setHtml(html.html());
}

void BookInfoDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::save(this, "BookInfoDialog");
    event->accept();
}
