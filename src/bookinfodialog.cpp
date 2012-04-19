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

    html.beginDivTag(".rawi-info");

    html.beginDivTag("#info");

    html.beginParagraphTag();
    html.insertSpanTag(tr("الكتاب:"), ".pro-name");
    html.insertSpanTag(m_book->title, ".pro-value");
    html.endParagraphTag();

    QString authorDeath;
    AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    if(author) {
        if(author->isALive)
            authorDeath = tr("(معاصر)");
        else if(!author->unknowDeath)
            authorDeath = tr("(%1)").arg(author->deathStr);
    }

    html.beginParagraphTag();
    html.insertSpanTag(tr("المؤلف:"), ".pro-name");
    html.beginSpanTag(".pro-value");
    html.insertAuthorLink(m_book->authorName + " " + authorDeath, m_book->authorID);
    html.endSpanTag();
    html.endParagraphTag();

    if(!m_book->edition.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الطبعة:"), ".pro-name");
        html.insertSpanTag(m_book->edition, ".pro-value");
        html.endParagraphTag();
    }

    if(!m_book->publisher.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الناشر:"), ".pro-name");
        html.insertSpanTag(m_book->publisher, ".pro-value");
        html.endParagraphTag();
    }

    if(!m_book->mohaqeq.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("المحقق:"), ".pro-name");
        html.insertSpanTag(m_book->mohaqeq, ".pro-value");
        html.endParagraphTag();
    }

    html.beginParagraphTag();
    html.insertSpanTag(tr("ترقيم الكتاب:"), ".pro-name");

    if(m_book->bookFlags & LibraryBook::PrintedPageNumber)
        html.insertSpanTag(tr("موافق للمطبوع"), ".pro-value");
    else if(m_book->bookFlags & LibraryBook::MakhetotPageNumer)
        html.insertSpanTag(tr("موافق للمخطوط"), ".pro-value");
    else
        html.insertSpanTag(tr("غير موافق للمطبوع"), ".pro-value");

    html.endParagraphTag();


    QString moqabal;
    if(m_book->bookFlags & LibraryBook::MoqabalMoteboa)
        moqabal += tr("المطبوع");
    else if(m_book->bookFlags & LibraryBook::MoqabalMakhetot)
        moqabal += tr("المخطوط");
    else if(m_book->bookFlags & LibraryBook::MoqabalPdf)
        moqabal += tr("نسخة مصورة PDF");

    if(!moqabal.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("مقابل على:"), ".pro-name");
        html.insertSpanTag(moqabal, ".pro-value");
        html.endParagraphTag();
    }

    QString otherInfo;
    if(m_book->bookFlags & LibraryBook::LinkedWithShareeh)
        otherInfo += tr("مرتبط بشرح، ");
    if(m_book->bookFlags & LibraryBook::HaveFootNotes)
        otherInfo += tr("مذيل بالحواشي، ");
    if(m_book->bookFlags & LibraryBook::Mashekool)
        otherInfo += tr("مشكول، ");

    if(!otherInfo.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("معلومات اخرى:"), ".pro-name");
        html.insertSpanTag(otherInfo, ".pro-value");
        html.endParagraphTag();
    }

    /*
    if(!m_book->comment.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("ملاحظات:"), ".pro-name");
        html.insertSpanTag(m_book->comment, ".pro-value");
        html.endParagraphTag();
    }
    */

    html.endDivTag(); // #info

    if(!m_book->info.isEmpty()) {
        html.insertHeadTag(4, tr("نبذة حول الكتاب"));
        html.insertDivTag(m_book->info, ".head-info");
    }

    html.endDivTag(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.addJSCode("setupToolTip();"
                   "moltaqaLink();");

    html.endAllTags();

    m_view->setHtml(html.html());
}

void BookInfoDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::savePosition(this, "BookInfoDialog");
    event->accept();
}
