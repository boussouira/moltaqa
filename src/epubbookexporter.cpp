#include "epubbookexporter.h"
#include "utils.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "ziphelper.h"
#include "xmldomhelper.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "xmldomhelper.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "htmlhelper.h"

#include <qtextstream.h>
#include <quuid.h>

EPubBookExporter::EPubBookExporter(QObject *parent) :
    BookExporter(parent)
{
}

void EPubBookExporter::start()
{
    openZip();

    init();
    writePages();
    writeContent();

    if(!m_book->isQuran()) {
        writeAuthorInfo();
        writeBookInfo();
        writeIntro();
    }

    writeTOC();

    closeZip();
}

void EPubBookExporter::openZip()
{
    QString bookPath = Utils::Rand::fileName(m_tempDir, "true", "book_", "epub");

    while(QFile::exists(bookPath)) {
        bookPath.replace(".epub", "_.epub");
    }

    m_zip.setZipName(bookPath);

    if(!m_zip.open(QuaZip::mdCreate))
        throw BookException("EPubBookExporter::openZip Can't creat zip file", bookPath, m_zip.getZipError());


    m_genereatedPath = bookPath;
}

void EPubBookExporter::closeZip()
{
    m_zip.close();

    if(m_zip.getZipError()!=0)
        throw BookException("EPubBookExporter::closeZip close zip", m_zip.getZipError());
}

void EPubBookExporter::init()
{
    m_sowarPages.clear();
    m_bookUID = QUuid::createUuid().toString().remove('{').remove('}');

    write("mimetype", "application/epub+zip");
    write("META-INF/container.xml", "<?xml version=\"1.0\"?>" "\n"
          "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">" "\n"
          "    <rootfiles>" "\n"
          "        <rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>" "\n"
          "    </rootfiles>" "\n"
          "</container>");

    write("OEBPS/Styles/main.css", "body { direction: rtl; text-align: right; margin: 2%;}" "\n"
          ".soraname { border: 1px solid #CCCCCC; color: #444444; font-size: 30px; "
          "margin-bottom: 25px; margin-left: 5px; margin-right: 5px; text-align: center; }" "\n"
          ".ayanumber { color: Teal; margin-left: 4px; margin-right: 4px; }" "\n"
          ".ayatxt { text-align: justify; }" "\n"
          ".bassemala { color: DarkBlue; margin-bottom: 10px; text-align: center; }" "\n"
          ".footnote { font-size: 0.8em; color: gray; }"
          ".footnote a { color: green; font-size: 0.9em; }"
          ".footnote p { margin: 0; padding: 0; }"
          ".footn { color: green; font-size: 0.8em; padding-bottom: 4px; }" "\n"
          "sanad { color: #000080; }  mateen { color: green; }  sheer { color: #273A9D; }" "\n"
          "hr.fns { width: 20%; float: right; }" "\n"
          ".center { text-align: center;}" "\n");
}

void EPubBookExporter::writePages()
{
    RichBookReader *reader = 0;
    if(m_book->isQuran()) {
        reader = new RichQuranReader();
    } else if(m_book->isNormal()) {
        RichSimpleBookReader *simple = new RichSimpleBookReader();
        simple->setShowShorooh(false);
        reader = simple;
    } else if(m_book->isTafessir()) {
        RichTafessirReader *taffesir = new RichTafessirReader();
        taffesir->setShowQuranText(false);
        reader = taffesir;
    } else {
        throw BookException(QString("EPubBookExporter::writePages Unknow book type %1").arg(m_book->type));
    }

    reader->setRemoveTashkil(m_removeTashkil);
    reader->setSaveReadingHistory(false);
    reader->setBookInfo(m_book);
    reader->openBook();

    BookPage *page = reader->page();

        while (reader->hasNext()) {
            reader->nextPage();

            if(page->text.isEmpty())
                continue;

            if(m_book->isQuran())
                m_sowarPages[page->sora] = page->pageID;

            writePage(page);
        }
}

void EPubBookExporter::writeBookInfo()
{
    ml_return_on_fail(m_book);

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.addCSS("Styles/main.css", true);
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

    html.endAll();

    QString xhtml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \n"
            "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";

    xhtml += html.html();
    xhtml += "\n</html>";


    write("OEBPS/book_info.xhtml", xhtml);
}

void EPubBookExporter::writeAuthorInfo()
{
    AuthorInfoPtr info = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    ml_return_on_fail(info);

    QList<LibraryBookPtr> books = LibraryManager::instance()->bookManager()->getAuthorBooks(m_book->authorID);

    HtmlHelper html;
    html.beginHead();
    html.setCharset();
    html.addCSS("Styles/main.css", true);
    html.setTitle(info->name);
    html.endHead();

    html.beginBody();

    html.beginDiv(".rawi-info");

    html.beginDiv(".nav");

    if(books.size())
        html.insertLink(tr("الكتب "), "#books");

    if(info->info.size())
        html.insertLink(tr("الترجمة"), "#tarejama");

    html.endDiv(); // .nav

    html.beginDiv("#info");
    html.beginDL(".dl-horizontal");

    if(info->name.size()) {
        html.insertDT(tr("الاسم: "));

        if(info->isALive)
            html.insertDD(tr("%1 (معاصر)").arg(info->name));
        else
            html.insertDD(info->name);

    }

    if(info->fullName.size()) {
        html.insertDT(tr("الاسم الكامل: "));
        html.insertDD(info->fullName);
    }

    if(info->birthStr.size()) {
        html.insertDT(tr("الولادة: "));
        html.insertDD(info->birthStr);
    }

    if(info->deathStr.size()) {
        html.insertDT(tr("الوفاة: "));
        html.insertDD(info->deathStr);
    }

    html.endDL(); // .dl-horizontal
    html.endDiv(); // #info

    if(books.size()) {
        html.insertHead(4, tr("كتب المؤلف (%1)").arg(books.size()), "#books");
        HtmlHelper bookHtml;

        for(int i=0; i<books.size(); i++) {
            if(i)
                bookHtml.insertBr();

            bookHtml.beginHtmlTag("a", "",
                                  QString("href='moltaqa://open/book?id=%1'").arg(books[i]->id));
            bookHtml.append(books[i]->title);
            bookHtml.endHtmlTag();

        }

        html.insertDiv(bookHtml.html(), ".head-info");
    }

    if(info->info.size()) {
        html.insertHead(4, tr("الترجمة"), "#tarejama");
        html.insertDiv(info->info, ".head-info");
    }

    html.endDiv(); // .rawi-info

    html.endAll();

    QString xhtml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \n"
            "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";

    xhtml += html.html();
    xhtml += "\n</html>";


    write("OEBPS/author_info.xhtml", xhtml);
}

void EPubBookExporter::writeIntro()
{
    HtmlHelper html;
    html.beginHead();
    html.setCharset();
    html.addCSS("../Styles/main.css", true);
    html.setTitle(m_book->title);
    html.endHead();

    html.beginBody();

    html.beginParagraph();
    html.append(tr("الكتاب: "));
    html.insertLink(m_book->title, "../book_info.xhtml");
    html.endParagraph();

    html.beginParagraph();
    html.append(tr("المؤلف: "));
    html.insertLink(m_book->authorName, "../author_info.xhtml");
    html.endParagraph();

    html.endAll();

    QString xhtml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \n"
            "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";

    xhtml += html.html();
    xhtml += "\n</html>";

    write("OEBPS/Text/intro.xhtml", xhtml);
}

void EPubBookExporter::writeContent()
{
    QuaZipFile file(&m_zip);
    if(file.open(QIODevice::WriteOnly, QuaZipNewInfo("OEBPS/content.opf"))) {
        QTextStream out(&file);
        out.setCodec("utf-8");

        out << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>" << "\n"
            << "<package xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"BookId\" version=\"2.0\">" << "\n"
            << "    <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">" << "\n"
            << "        <dc:title>" << m_book->title << "</dc:title>" << "\n"
            << "        <dc:creator opf:role=\"aut\">" << m_book->authorName << "</dc:creator>" << "\n"
            << "        <dc:publisher>" << tr("مكتبة الملتقى") << "</dc:publisher>" << "\n"
            << "        <dc:language>ar</dc:language>" << "\n"
            << "        <dc:subject>Islam, Religion</dc:subject>" << "\n"
            << "        <dc:identifier id=\"BookId\" opf:scheme=\"UUID\">" << m_bookUID << "</dc:identifier>" << "\n"
            << "        <meta content=\"" << App::version() << "\" name=\"Moltaqa library\" />" << "\n"
            << "    </metadata>" << "\n"
            << "    <manifest>" << "\n"
            << "        <item href=\"toc.ncx\" id=\"ncx\" media-type=\"application/x-dtbncx+xml\" />" << "\n"
            << "        <item id=\"stylesheet\" href=\"Styles/main.css\" media-type=\"text/css\" />" << "\n"
            << "        <item id=\"author_info\" href=\"author_info.xhtml\" media-type=\"application/xhtml+xml\" />" << "\n"
            << "        <item id=\"book_info\" href=\"book_info.xhtml\" media-type=\"application/xhtml+xml\" />" << "\n";

        if(!m_book->isQuran())
            out << "        <item id=\"intro\" href=\"Text/intro.xhtml\" media-type=\"application/xhtml+xml\" />" << "\n";

        foreach (QString p, m_page) {
            out << "        "
                << QString("<item id=\"%1\" href=\"Text/%1.xhtml\" media-type=\"application/xhtml+xml\" />").arg(p)
                << "\n";
        }

        out << "    </manifest>" << "\n"
            << "    <spine toc=\"ncx\">" << "\n";

        if(!m_book->isQuran())
            out << "        <itemref idref=\"intro\" />" << "\n";

        foreach (QString p, m_page) {
            out << "        <itemref idref=\"" << p << "\" />" << "\n";
        }

        out << "    </spine>" << "\n"
            << "    <guide>" << "\n"
            << "        <reference type=\"bibliography\" title=\"Author info\" href=\"Text/author_info.xhtml\" />" << "\n"
            << "        <reference type=\"title-page\" title=\"Book info\" href=\"Text/book_info.xhtml\" />" << "\n"
            << "    </guide>" << "\n"
            << "</package>";

        out.flush();
        file.close();
    } else {
        throw BookException("writePage::writeContent error when writing to OEBPS/content.opf", file.getZipError());
    }
}

void EPubBookExporter::writeTOC()
{
    QuaZipFile file(&m_zip);
    if(file.open(QIODevice::WriteOnly, QuaZipNewInfo("OEBPS/toc.ncx"))) {
        QTextStream out(&file);
        out.setCodec("utf-8");

        out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << "\n"
            << "<!DOCTYPE ncx PUBLIC \"-//NISO//DTD ncx 2005-1//EN\" " << "\n"
            << "   \"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">" << "\n"
            << "<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">" << "\n"
            << "<head>" << "\n"
            << "   <meta name=\"dtb:uid\" content=\"" << m_bookUID << "\" />" << "\n"
            << "   <meta name=\"dtb:depth\" content=\"1\" />" << "\n"
            << "   <meta name=\"dtb:totalPageCount\" content=\"0\" />" << "\n"
            << "   <meta name=\"dtb:maxPageNumber\" content=\"0\" />" << "\n"
            << "</head>" << "\n"
            << "<docTitle>" << "\n"
            << "   <text>" << m_book->title << "</text>" << "\n"
            << "</docTitle>" << "\n"
            << "<docAuthor>" << "\n"
            << "   <text>" << m_book->authorName << "</text>" << "\n"
            << "</docAuthor>" << "\n"

            << "<navMap>" << "\n";

        m_titleCount = 0;

        if(m_book->isQuran())
            writeQuranBookTOC(out);
        else
            writeSimpleBookTOC(out);

        out << "</navMap>" << "\n"
            << "</ncx>";

        out.flush();
        file.close();
    } else {
        throw BookException("EPubBookExporter::writeTOC error when writing to OEBPS/toc.ncx", file.getZipError());
    }
}

void EPubBookExporter::writeSimpleBookTOC(QTextStream &out)
{

    QuaZip bookZip(m_book->path);
    if(!bookZip.open(QuaZip::mdUnzip)) {
        throw BookException("EPubBookExporter::writeTOC error when book",
                            m_book->path);
    }

    QuaZipFile titleFile(&bookZip);

    if(! (bookZip.setCurrentFile("titles.xml")
          && titleFile.open(QIODevice::ReadOnly))) {
        throw BookException("EPubBookExporter::writeTOC error when opening titles.xml",
                            titleFile.getZipError());
    }

    XmlDomHelper helper;
    helper.load(&titleFile);

    QDomElement element = helper.rootElement().firstChildElement();
    while(!element.isNull()) {
        writeTocItem(element, out);

        element = element.nextSiblingElement();
    }
}

void EPubBookExporter::writeQuranBookTOC(QTextStream &out)
{
    for(int i=1; i<=114; i++) {
        QuranSora *sora = MW->readerHelper()->getQuranSora(i);
        if(sora) {
            m_titleCount++;

            out << "<navPoint id=\"nav_" << m_titleCount << "\" playOrder=\"" << m_titleCount << "\">" << "\n";
            out << "<navLabel><text>" << sora->name << "</text></navLabel>" << "\n";
            out << "<content src=\"Text/page_" << m_sowarPages[i]+1 << ".xhtml\"/>" << "\n";
            out << "</navPoint>" << "\n";
        }
    }
}

void EPubBookExporter::writeTocItem(QDomElement &element, QTextStream &out)
{
    int pageID = element.attribute("pageID").toInt();
    m_titleCount++;

    out << "<navPoint id=\"nav_" << m_titleCount << "\" playOrder=\"" << m_titleCount << "\">" << "\n";
    out << "<navLabel><text>" << element.firstChildElement("text").text() << "</text></navLabel>" << "\n";
    out << "<content src=\"Text/page_" << pageID << ".xhtml\"/>" << "\n";

    if(element.childNodes().count() > 1) {
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull()) {
            writeTocItem(child, out);

            child = child.nextSiblingElement("title");
        }
    }

    out << "</navPoint>" << "\n";
}

void EPubBookExporter::write(const QString &fileName, const QString &data)
{
    QuaZipFile file(&m_zip);
    if(file.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName))) {
        QTextStream out(&file);
        out.setCodec("utf-8");
        out << data;
        out.flush();

        file.close();
    } else {
        throw BookException("EPubBookExporter::write error when writing to info.xml", file.getZipError());
    }
}

void EPubBookExporter::writePage(BookPage *page)
{
    QString fileName = QString("page_%1").arg(page->pageID);
    QuaZipFile file(&m_zip);
    if(file.open(QIODevice::WriteOnly, QuaZipNewInfo("OEBPS/Text/" + fileName + ".xhtml"))) {
        QTextStream out(&file);
        out.setCodec("utf-8");

        out << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" << "\n";
        out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" " << "\n"
            << "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << "\n\n";

        out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << "\n";

        out << "<head>" << "\n";
        out << "<title></title>" << "\n";
        out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>" << "\n";
        out << "<link href=\"../Styles/main.css\" rel=\"stylesheet\" type=\"text/css\" />" << "\n";
        out << "</head>" << "\n\n";

        out << "<body>" << "\n";

        if(!page->text.contains("<div class=\"clear\"></div>"))
            page->text.replace("<div class=\"clear\">", "<div class=\"clear\"></div>");

        if(page->text.contains("<footnote>")) {
            page->text.replace("</footnote>", "</div>");
            page->text.replace("<footnote>", "<div class=\"footnote\">");
        }

        out << page->text << "\n";

        if(m_addPageNumber && !m_book->isQuran()) {
            out << "<p class=\"center\">"
                << tr("الصفحة: ") << page->page << " - "
                << tr("الجزء: ") << page->part
                << "</p>" << "\n";
        }

        out << "</body>\n</html>";

        out.flush();
        file.close();

        m_page.append(fileName);
    } else {
        throw BookException("writePage::write error when writing to info.xml", file.getZipError());
    }
}
