#include "htmlbookexporter.h"
#include "utils.h"
#include "librarymanager.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "bookreaderhelper.h"
#include "htmlhelper.h"
#include "ziphelper.h"
#include "xmldomhelper.h"
#include "xmlutils.h"
#include "stringutils.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "bookutils.h"

#include <qtextstream.h>

HtmlBookExporter::HtmlBookExporter(QObject *parent) :
    BookExporter(parent)
{
    m_reader = 0;
}

void HtmlBookExporter::start()
{
    openReader();
    writePages();
    writeTOC();
}

void HtmlBookExporter::openReader()
{
    QDir dir(m_outDir);
    if(!dir.exists("style")) {
        dir.mkdir("style");
        copyStyleTemplate(dir.filePath("style"));
    }

    m_genereatedPath = dir.filePath(m_book->title);

    while(QFile::exists(m_genereatedPath)) {
        m_genereatedPath.append("_");
    }

    dir.mkdir(m_genereatedPath);
    dir.cd(m_genereatedPath);
    dir.mkdir("pages");

    if(m_book->isQuran()) {
        m_reader = new RichQuranReader();
    } else if(m_book->isNormal()) {
        RichSimpleBookReader *simple = new RichSimpleBookReader();
        simple->setShowShorooh(false);
        m_reader = simple;
    } else if(m_book->isTafessir()) {
        RichTafessirReader *taffesir = new RichTafessirReader();
        taffesir->setShowQuranText(false);
        m_reader = taffesir;
    } else {
        throw BookException(QString("HtmlBookExporter::openReader Unknow book type %1").arg(m_book->type));
    }

    m_reader->setRemoveTashkil(m_removeTashkil);
    m_reader->setSaveReadingHistory(false);
    m_reader->setBookInfo(m_book);
    m_reader->openBook();
    m_reader->loadPages();
}

void HtmlBookExporter::copyStyleTemplate(QString styleDirPath)
{
    QDir dir(App::shareDir());
    dir.cd("template");
    dir.cd("html_export");

    QDir styleDir(styleDirPath);
    QStringList list = dir.entryList(QDir::Files|QDir::NoDotAndDotDot);
    foreach (QString file, list) {
        QFile::copy(dir.filePath(file), styleDir.filePath(file));
    }
}

void HtmlBookExporter::writeTOC()
{
    QFile file(QString("%1/index.html").arg(m_genereatedPath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw BookException("HtmlBookExporter::writeTOC can't open file for writing",
                            file.fileName());

    QTextStream out(&file);
    out.setCodec("utf-8");
    out << "<!DOCTYPE html>"
        << "<html>"
        << "<head>"
        << "<link href= \"../style/default.css\" rel=\"stylesheet\" type=\"text/css\"/>"
        << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
        << "<title>" << tr("كتاب") << " "
        << m_book->title
        << "</title></head><body>";

    out << "<ul>""\n";

    if(m_book->isQuran()) {
        for(int i=1; i<=114; i++) {
            QuranSora *sora = MW->readerHelper()->getQuranSora(i);
            if(sora) {
                out << "<li><a href=\"pages/p"
                    << m_sowarPages[i]+1 << ".html\">"
                    << sora->name << "</a></li>" << "\n";
            }
        }
    } else {
        QuaZip zip;
        zip.setZipName(m_book->path);

        if(!zip.open(QuaZip::mdUnzip)) {
            throw BookException(QString("HtmlBookExporter::writeTOC open book error %\n" "Title: %2\n" "Path: %3")
                                .arg(zip.getZipError()).arg(m_book->title).arg(m_book->path));
        }

        QuaZipFile titleFile(&zip);

        if(zip.setCurrentFile("titles.xml")) {
            if(!titleFile.open(QIODevice::ReadOnly)) {
                throw BookException("HtmlBookExporter::writeTOC open title file error",
                                    titleFile.getZipError());
            }
        }

        QDomDocument doc = Utils::Xml::getDomDocument(&titleFile);
        if(doc.isNull())
            throw BookException("HtmlBookExporter::writeTOC null DOM document");

        QDomElement root = doc.documentElement();
        QDomElement element = root.firstChildElement();

        while(!element.isNull() && !m_stop) {
            writeTocItem(out, element);

            element = element.nextSiblingElement();
        }

        qSort(m_pageTitles);
    }

    out << "</ul></body></html>";
}

void HtmlBookExporter::writeTocItem(QTextStream &out, QDomElement &element)
{
    int pageID = element.attribute("pageID").toInt();
    QString text = element.firstChildElement("text").text();
    if(m_removeTashkil)
        text = Utils::String::Arabic::removeTashekil(text);

    out << "<li><a id=\"id_"
        << pageID << "\" href=\"pages/p"
        << pageID << ".html\">"
        << text << "</a></li>"
        << "\n";

    if(element.childNodes().count() > 1) {
        out << "<ul>" << "\n";
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull() && !m_stop) {
            writeTocItem(out, child);

            child = child.nextSiblingElement("title");
        }
        out << "</ul>" << "\n";
    }

    m_pageTitles << pageID;
}

void HtmlBookExporter::writePages()
{
    BookPage *page = m_reader->page();

    QDir dir(m_genereatedPath);
    dir.cd("pages");

    while (m_reader->hasNext()) {
        m_reader->nextPage();

        if(page->text.isEmpty())
            continue;

        if(m_book->isQuran())
            m_sowarPages[page->sora] = page->page;

        writePage(dir, page);
    }
}

void HtmlBookExporter::writePage(QDir &dir, BookPage *page)
{
    QString fileName = dir.filePath(QString("p%1.html").arg(m_book->isQuran() ? page->page : page->pageID));
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        throw BookException("HtmlBookExporter::writePage error when writing page text",
                            file.errorString() + ": " + file.fileName());
    }

    HtmlHelper helper;
    helper.beginHtml();

    helper.beginHead();
    helper.setCharset("utf-8");
    helper.addCSS("../../style/default.css", true);

    helper.endHead();

    helper.beginBody();
    if(m_book->isQuran())
        helper.beginDiv("#quran");
    else
        helper.beginDiv("#simpleBook");

    if(!m_book->isQuran()) {
        helper.beginDiv("#pageHeader");

        helper.beginDiv(".bookInfo");
        helper.insertSpan(m_book->title, ".bookName");

        helper.beginDiv("#currentPage");

        helper.beginSpan(".page");
        helper.insertSpan(tr("الصفحة "), ".name");
        helper.insertSpan(QString::number(page->page), ".val");
        helper.endSpan(); // span.page

        helper.beginSpan(".part");
        helper.insertSpan(tr(" الجزء "), ".name");
        helper.insertSpan(QString::number(page->part), ".val");
        helper.endSpan(); // span.part

        helper.endDiv(); // div#currentPage
        helper.endDiv(); // div.bookInfo
        helper.endDiv(); // div#pageHeader
    }

    helper.beginDiv("#pageText");
    helper.append(page->text);
    helper.endDiv();

    helper.endDiv(); // div#m_cssID

    helper.beginDiv("#pageNav");

    if(m_reader->hasPrev()) {
        helper.beginLink(QString("p%1.html").arg(m_reader->prevPageID()));
        helper.insertImage("../../style/go-next.png");
        helper.endLink();
    }

    helper.beginLink(QString("../index.html#id_%1").arg(Utils::Book::getPageTitleID(m_pageTitles, page->pageID)));
    helper.insertImage("../../style/go-up.png");
    helper.endLink();

    if(m_reader->hasNext()) {
        helper.beginLink(QString("p%1.html").arg(m_reader->nextPageID()));
        helper.insertImage("../../style/go-previous.png");
        helper.endLink();
    }

    helper.endDiv();
    helper.endAll();

    QTextStream out(&file);
    out.setCodec("utf-8");

    out << helper.html();

    out.flush();
    file.close();
}
