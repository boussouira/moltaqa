#include "epubbookexporter.h"
#include "authorsmanager.h"
#include "bookfilesreader.h"
#include "bookreaderhelper.h"
#include "htmlhelper.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "mimeutils.h"
#include "qurantextformat.h"
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "utils.h"
#include "xmldomhelper.h"
#include "ziphelper.h"

#include <qtextstream.h>

#define IMAGES_PREFIX "images/"
#define IMAGES_PREFIX_size 7

class EPubImageReader : public BookFilesReader
{
public:
    EPubImageReader(EPubBookExporter *exporter) : m_exporter(exporter) {}

    bool acceptFile(QString filePath)
    {
        return filePath.startsWith(IMAGES_PREFIX)
                && filePath.size() > IMAGES_PREFIX_size;
    }

    bool readFile(QString filePath, QIODevice &file)
    {
        QByteArray out;
        Utils::Files::copyData(file, out);

        m_exporter->m_zipWriter.add(QString("OEBPS/%1").arg(filePath), out,
                                    ZipWriterManager::Bottom);

        m_exporter->m_images.insert(QString(filePath).replace(IMAGES_PREFIX, "x"),
                                    filePath);

        return true;
    }

    EPubBookExporter *m_exporter;
};

EPubBookExporter::EPubBookExporter(QObject *parent) :
    BookExporter(parent)
{
    m_bookHasImages = false;
}

void EPubBookExporter::start()
{
    openZip();

    init();
    writePages();

    if(m_bookHasImages)
        writeImages();

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
    QString bookPath = Utils::Rand::fileName(m_tempDir, true, "book_", "epub");

    while(QFile::exists(bookPath)) {
        bookPath.replace(".epub", "_.epub");
    }

    if(!m_zipWriter.open(bookPath))
        throw BookException("EPubBookExporter::openZip Can't creat zip file", bookPath);


    m_genereatedPath = bookPath;
}

void EPubBookExporter::closeZip()
{
    if(!m_zipWriter.close())
        throw BookException("EPubBookExporter::closeZip close zip", m_zipWriter.zipPath());
}

void EPubBookExporter::init()
{
    m_sowarPages.clear();
    m_bookUID = Utils::Rand::uuid();

    write("mimetype", "application/epub+zip", true);
    write("META-INF/container.xml", "<?xml version=\"1.0\"?>" "\n"
          "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">" "\n"
          "    <rootfiles>" "\n"
          "        <rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>" "\n"
          "    </rootfiles>" "\n"
          "</container>", true);

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
          "div.clear { clear: both; }" "\n"
          ".center { text-align: center;}" "\n", true);
}

void EPubBookExporter::writePages()
{
    RichBookReader *reader = 0;
    if(m_book->isQuran()) {
        RichQuranReader *quran = new RichQuranReader();
        quran->quranFormatter()->setDrawAyaNumber(false);
        reader = quran;
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
    reader->loadPages();

    BookPage *page = reader->page();

        while (reader->hasNext()) {
            reader->nextPage();

            if(page->text.isEmpty())
                continue;

            if(m_book->isQuran() && !m_sowarPages.contains(page->sora))
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
    html.addCSS("../Styles/main.css", true);
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

    if(m_book->comment.size() && !m_book->comment.contains(tr("[[المكتبة الشاملة]]"))) {
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


    write("OEBPS/book_info.xhtml", xhtml, true);
}

void EPubBookExporter::writeAuthorInfo()
{
    AuthorInfo::Ptr info = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    ml_return_on_fail(info);

    QList<LibraryBook::Ptr> books = LibraryManager::instance()->bookManager()->getAuthorBooks(m_book->authorID);

    HtmlHelper html;
    html.beginHead();
    html.setCharset();
    html.addCSS("../Styles/main.css", true);
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
                                  QString("href='moltaqa://?c=open&amp;t=book&amp;id=%1'").arg(books[i]->uuid));
            bookHtml.appendText(books[i]->title);
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


    write("OEBPS/author_info.xhtml", xhtml, true);
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
    html.appendText(tr("الكتاب: "));
    html.insertLink(m_book->title, "book_info.xhtml");
    html.endParagraph();

    html.beginParagraph();
    html.appendText(tr("المؤلف: "));
    html.insertLink(m_book->authorName, "author_info.xhtml");
    html.endParagraph();

    html.endAll();

    QString xhtml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \n"
            "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";

    xhtml += html.html();
    xhtml += "\n</html>";

    write("OEBPS/intro.xhtml", xhtml, true);
}

void EPubBookExporter::writeContent()
{
    QString filePath = Utils::Rand::fileName(m_tempDir, true, "content_", "opf");

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly))
        throw BookException("writePage::writeContent error when writing to OEBPS/content.opf", filePath);

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
        out << "        <item id=\"intro\" href=\"intro.xhtml\" media-type=\"application/xhtml+xml\" />" << "\n";

    foreach (QString p, m_page) {
        out << "        "
            << QString("<item id=\"%1\" href=\"%1.xhtml\" media-type=\"application/xhtml+xml\" />").arg(p)
            << "\n";
    }

    QHashIterator<QString, QString> i(m_images);
     while (i.hasNext()) {
         i.next();
         out << "        "
             << "<item id=\"" << i.key() << "\" href=\"" << i.value()
             << "\" media-type=\"" << Utils::Mimes::fileTypeFromFileName(i.key()) << "\" />" << "\n";
     }

    out << "    </manifest>" << "\n"
        << "    <spine toc=\"ncx\">" << "\n";

    if(!m_book->isQuran())
        out << "        <itemref idref=\"intro\" linear=\"yes\" />" << "\n";

    foreach (QString p, m_page) {
        out << "        <itemref idref=\"" << p << "\" linear=\"yes\" />" << "\n";
    }

    if(!m_book->isQuran()) {
        out << "        <itemref idref=\"book_info\" linear=\"yes\" />" << "\n";
        out << "        <itemref idref=\"author_info\" linear=\"yes\" />" << "\n";
    }

    out << "    </spine>" << "\n"
        << "    <guide>" << "\n"
        << "        <reference type=\"bibliography\" title=\"Author info\" href=\"author_info.xhtml\" />" << "\n"
        << "        <reference type=\"title-page\" title=\"Book info\" href=\"book_info.xhtml\" />" << "\n";

//    foreach (QString p, m_page) {
//        out << "        " << QString("<reference type=\"text\" title=\"%1\" href=\"%1.xhtml\" />").arg(p) << "\n";
//    }

    out << "    </guide>" << "\n"
        << "</package>";

    out.flush();
    file.close();

    m_zipWriter.addFromFile("OEBPS/content.opf", filePath, ZipWriterManager::Top);

    QFile::remove(filePath);
}

void EPubBookExporter::writeTOC()
{
    QString filePath = Utils::Rand::fileName(m_tempDir, true, "toc_", "ncx");

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly))
        throw BookException("writePage::writeTOC error when writing to OEBPS/toc.ncx", filePath);

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

    m_zipWriter.addFromFile("OEBPS/toc.ncx", filePath, ZipWriterManager::Top);

    QFile::remove(filePath);
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

            int k = i;
            int pageNum = m_sowarPages[k];
            while(!pageNum && k >= 0) {
                pageNum = m_sowarPages[--k];
            }

            out << "<navPoint id=\"nav_" << m_titleCount << "\" playOrder=\"" << m_titleCount << "\">" << "\n";
            out << "<navLabel><text>" << sora->name << "</text></navLabel>" << "\n";
            out << "<content src=\"page_" << pageNum << ".xhtml\"/>" << "\n";
            out << "</navPoint>" << "\n";
        }
    }
}

void EPubBookExporter::writeTocItem(QDomElement &element, QTextStream &out)
{
    QString tid = element.attribute("tagID");
    int pageID = element.attribute("pageID").toInt();
    m_titleCount++;

    out << "<navPoint id=\"nav_" << m_titleCount << "\" playOrder=\"" << m_titleCount << "\">" << "\n";
    out << "<navLabel><text>" << element.firstChildElement("text").text() << "</text></navLabel>" << "\n";
    out << "<content src=\"page_" << m_containerPages.value(pageID) << ".xhtml#" << tid << "\"/>" << "\n";

    if(element.childNodes().count() > 1) {
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull()) {
            writeTocItem(child, out);

            child = child.nextSiblingElement("title");
        }
    }

    out << "</navPoint>" << "\n";
}

void EPubBookExporter::write(const QString &fileName, const QString &data, bool prepend)
{
    ZipWriterManager::InsertOrder order = (prepend ? ZipWriterManager::Top : ZipWriterManager::Bottom);
    m_zipWriter.add(fileName, data.toUtf8(), order);
}

void EPubBookExporter::writePage(BookPage *page)
{
    QString fileName = QString("page_%1").arg(page->pageID);
    QString out;

    out += "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" "\n";
    out += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" " "\n"
            "    \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" "\n\n";

    out += "<html xmlns=\"http://www.w3.org/1999/xhtml\">" "\n";

    out += "<head>" "\n";
    out += "<title></title>" "\n";
    out += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>" "\n";
    out += "<link href=\"../Styles/main.css\" rel=\"stylesheet\" type=\"text/css\" />" "\n";
    out += "</head>" "\n\n";

    out += "<body>" "\n";

    QRegExp rx("src\\s*=\\s*\"images/");
    if(m_bookHasImages || page->text.contains(rx)) {
        page->text.replace(rx, "src=\"../images/");
        m_bookHasImages = true;
    }

    out += page->text;

    if(m_addPageNumber && !m_book->isQuran()
            && page->page && page->part) {
        out += "\n" "<p class=\"center\">";
        out += tr("الصفحة: %1").arg(page->page);
        out += " - ";
        out += tr("الجزء: %1").arg(page->part);
        out += "</p>" "\n";
    }

    out += "\n" "</body>\n</html>";

    write(QString("OEBPS/%1.xhtml").arg(fileName), out, false);
    m_page.append(fileName);
}

void EPubBookExporter::writeImages()
{
    EPubImageReader reader(this);
    reader.setBook(m_book);
    reader.start();
}
