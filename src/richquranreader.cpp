#include "richquranreader.h"
#include "librarybook.h"
#include "bookexception.h"
#include "qurantextformat.h"
#include "libraryinfo.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "modelenums.h"
#include "utils.h"

RichQuranReader::RichQuranReader(QObject *parent) : RichBookReader(parent)
{
    m_formatter = new QuranTextFormat();
    m_textFormat = m_formatter;
}

RichQuranReader::~RichQuranReader()
{
}

void RichQuranReader::setCurrentPage(QDomElement pageNode)
{
    m_formatter->start();

    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->sora = pageNode.attribute("sora").toInt();
    m_currentPage->aya = pageNode.attribute("aya").toInt();
    m_currentPage->ayatCount = MW->readerHelper()->getQuranSora(m_currentPage->sora)->ayatCount;
    m_currentPage->titleID = m_currentPage->sora;

    m_pagesDom.setCurrentElement(pageNode);

    if(pageNode.attribute("page").toInt() == m_currentPage->page) {
        updateHistory();
        return;
    }

    QDomElement prevNode = pageNode.previousSiblingElement();
    while(!prevNode.isNull()) {
        if(prevNode.attribute("page") == pageNode.attribute("page")) {
            pageNode = prevNode;
        } else {
            break;
        }

        prevNode = prevNode.previousSiblingElement();
    }

    m_currentPage->page = pageNode.attribute("page").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();

    QDomElement ayaNode = pageNode;

    while(!ayaNode.isNull() && ayaNode.attribute("page").toInt() == m_currentPage->page) {
        int id = ayaNode.attribute("id").toInt();
        int sora = ayaNode.attribute("sora").toInt();
        int aya = ayaNode.attribute("aya").toInt();
        QString text = ayaNode.text();

        // at the first vers we insert the sora name and bassemala
        if(aya == 1) {
            m_formatter->insertSoraName(MW->readerHelper()->getQuranSora(sora)->name);

            // we escape putting bassemala before Fateha and Tawba
            if(sora != 1 && sora != 9)
                m_formatter->insertBassemala();
        }

        if(m_query && m_highlightPageID == id)
            text = Utils::CLucene::highlightText(text, m_query, false);

        m_formatter->insertAyaText(text, aya, sora);

        ayaNode = ayaNode.nextSiblingElement();
    }

    m_formatter->done();

    emit textChanged();
}

QStandardItemModel *RichQuranReader::indexModel()
{
    m_indexModel = new QStandardItemModel();

    for(int i=1; i<=114; i++) {
        QuranSora *sora = MW->readerHelper()->getQuranSora(i);
        if(sora) {
            QStandardItem *soraNode = new QStandardItem(sora->name);
            soraNode->setData(i, ItemRole::idRole);
            m_indexModel->appendRow(soraNode);
        }
    }

    return m_indexModel;
}

void RichQuranReader::nextPage()
{
    QDomElement e = m_pagesDom.currentElement().nextSiblingElement();

    while(!e.isNull()) {
        if(e.attribute("page") != m_pagesDom.currentElement().attribute("page")) {
            setCurrentPage(e);
            break;
        }

        e = e.nextSiblingElement();
    }
}

void RichQuranReader::prevPage()
{
    QDomElement e = m_pagesDom.currentElement().previousSiblingElement();

    while(!e.isNull()) {
        if(e.attribute("page") != m_pagesDom.currentElement().attribute("page")) {
            QDomElement t = e;
            while(!t.isNull()) {
                if(t.attribute("page") == e.attribute("page"))
                    e = t;
                else
                    break;


                t = t.previousSiblingElement();
            }

            setCurrentPage(e);
            break;
        }

        e = e.previousSiblingElement();
    }
}

void RichQuranReader::nextAya()
{
    QDomElement e = m_pagesDom.currentElement().nextSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}

void RichQuranReader::prevAya()
{
    QDomElement e = m_pagesDom.currentElement().previousSiblingElement();

    if(!e.isNull())
        setCurrentPage(e);
}
