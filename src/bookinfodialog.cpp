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
    layout->addWidget(m_view);

    Utils::Widget::restorePosition(this, "BookInfoDialog");
}

void BookInfoDialog::setup()
{
    ML_ASSERT2(m_book, "BookInfoDialog::setup book is null");

    setWindowTitle(m_book->title);

    QDir styleDir(App::stylesDir());
    styleDir.cd("default");

    QString style = QUrl::fromLocalFile(styleDir.filePath("default.css")).toString();

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.addCSS(style);
    html.setTitle(m_book->title);
    html.endHead();

    html.beginBody();

    html.beginDiv(".rawi-info");

    html.beginDiv("#info");

    html.beginParagraph();
    html.insertSpan(tr("الكتاب:"), ".pro-name");
    html.insertSpan(m_book->title, ".pro-value");
    html.endParagraph();

    QString authorDeath;
    AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    if(author) {
        if(author->isALive)
            authorDeath = tr("(معاصر)");
        else if(!author->unknowDeath)
            authorDeath = tr("(%1)").arg(author->deathStr);
    }

    html.beginParagraph();
    html.insertSpan(tr("المؤلف:"), ".pro-name");
    html.beginSpan(".pro-value");
    html.insertAuthorLink(m_book->authorName + " " + authorDeath, m_book->authorID);
    html.endSpan();
    html.endParagraph();

    if(!m_book->edition.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("الطبعة:"), ".pro-name");
        html.insertSpan(m_book->edition, ".pro-value");
        html.endParagraph();
    }

    if(!m_book->publisher.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("الناشر:"), ".pro-name");
        html.insertSpan(m_book->publisher, ".pro-value");
        html.endParagraph();
    }

    if(!m_book->mohaqeq.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("المحقق:"), ".pro-name");
        html.insertSpan(m_book->mohaqeq, ".pro-value");
        html.endParagraph();
    }

    html.beginParagraph();
    html.insertSpan(tr("ترقيم الكتاب:"), ".pro-name");

    if(m_book->bookFlags & LibraryBook::PrintedPageNumber)
        html.insertSpan(tr("موافق للمطبوع"), ".pro-value");
    else if(m_book->bookFlags & LibraryBook::MakhetotPageNumer)
        html.insertSpan(tr("موافق للمخطوط"), ".pro-value");
    else
        html.insertSpan(tr("غير موافق للمطبوع"), ".pro-value");

    html.endParagraph();


    QString moqabal;
    if(m_book->bookFlags & LibraryBook::MoqabalMoteboa)
        moqabal += tr("المطبوع");
    else if(m_book->bookFlags & LibraryBook::MoqabalMakhetot)
        moqabal += tr("المخطوط");
    else if(m_book->bookFlags & LibraryBook::MoqabalPdf)
        moqabal += tr("نسخة مصورة PDF");

    if(!moqabal.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("مقابل على:"), ".pro-name");
        html.insertSpan(moqabal, ".pro-value");
        html.endParagraph();
    }

    QString otherInfo;
    if(m_book->bookFlags & LibraryBook::LinkedWithShareeh)
        otherInfo += tr("مرتبط بشرح، ");
    if(m_book->bookFlags & LibraryBook::HaveFootNotes)
        otherInfo += tr("مذيل بالحواشي، ");
    if(m_book->bookFlags & LibraryBook::Mashekool)
        otherInfo += tr("مشكول، ");

    if(!otherInfo.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("معلومات اخرى:"), ".pro-name");
        html.insertSpan(otherInfo, ".pro-value");
        html.endParagraph();
    }

    /*
    if(!m_book->comment.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("ملاحظات:"), ".pro-name");
        html.insertSpanTag(m_book->comment, ".pro-value");
        html.endParagraphTag();
    }
    */

    html.endDiv(); // #info

    if(!m_book->info.isEmpty()) {
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
    Utils::Widget::savePosition(this, "BookInfoDialog");
    event->accept();
}