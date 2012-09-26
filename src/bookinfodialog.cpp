#include "bookinfodialog.h"
#include "htmlhelper.h"
#include "utils.h"
#include "authorsmanager.h"
#include "librarymanager.h"
#include <qboxlayout.h>

BookInfoDialog::BookInfoDialog(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_view = new WebView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);

    Utils::Widget::restore(this, "BookInfoDialog");
}

void BookInfoDialog::setup()
{
    ml_return_on_fail2(m_book, "BookInfoDialog::setup book is null");

    setWindowTitle(m_book->title);

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.addCSS("default.css");
    html.setTitle(m_book->title);
    html.endHead();

    html.beginBody();

    html.beginDiv(".rawi-info");

    html.beginDiv("#info");
    html.beginDL(".dl-horizontal");

    html.insertDT(tr("الكتاب:"));
    html.insertDD(m_book->title);

    if(!m_book->isQuran()) {
        QString authorDeath;
        AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
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

    if(m_book->comment.size() && !m_book->comment.contains(tr("[مأخود من الشاملة]"))) {
        html.insertDT(tr("ملاحظات:"));
        html.insertDD(m_book->comment);
    }

    html.endDL(); // .dl-horizontal
    html.endDiv(); // #info

    if(m_book->info.size()) {
        html.insertHead(4, tr("نبذة حول الكتاب"));
        html.insertDiv(m_book->info, ".head-info");
    }

    html.endDiv(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.addJSCode("setupToolTip();"
                   "moltaqaLink();");

    html.endAll();

    m_view->setHtml(html.html());
}

void BookInfoDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::save(this, "BookInfoDialog");
    event->accept();
}
