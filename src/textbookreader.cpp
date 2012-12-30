#include "textbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include <qxmlstream.h>

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
    m_loadTitlesText = false;
}

TextBookReader::~TextBookReader()
{
    m_titles.clear();
    m_pages.clear();
}

void TextBookReader::firstPage()
{
    AbstractBookReader::firstPage();

    m_currentPage->titleID = m_titles.first();
}

void TextBookReader::load()
{
    ml_return_on_fail(!m_bookInfo->isQuran());

    loadTitles();
    loadPages();
}

void TextBookReader::loadTitles()
{
    ZipOpener opener(this);
    QuaZipFile titleFile(&m_zip);

    ml_return_on_fail2(m_zip.setCurrentFile("titles.xml"), "getTitles: setCurrentFile error" << titleFile.getZipError());
    ml_return_on_fail2(titleFile.open(QIODevice::ReadOnly), "getTitles: open error" << titleFile.getZipError());

    QXmlStreamReader reader(&titleFile);
    int titleID = -1;

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "title") {
                titleID = reader.attributes().value("pageID").toString().toInt();
                m_titles.append(titleID);
            } else if(m_loadTitlesText && reader.name() == "text") {
                if(reader.readNext() == QXmlStreamReader::Characters) {
                    if(m_titlesText.contains(titleID))
                        m_titlesText[titleID].append(' ').append(reader.text().toString());
                    else
                        m_titlesText[titleID] = reader.text().toString();
                } else {
                    if(reader.tokenType() != QXmlStreamReader::EndElement) { // Ignore empty titles
                        qWarning() << "TextBookReader::getTitles Unexpected token type" << reader.tokenString()
                                   << "- Book:" << m_bookInfo->id << m_bookInfo->title << m_bookInfo->fileName;
                        break;
                    }
                }
            }
        }

        if(reader.hasError()) {
            qDebug() << "getTitles: QXmlStreamReader error:" << reader.errorString()
                     << "- Book:" << m_bookInfo->id << m_bookInfo->title << m_bookInfo->fileName;
            break;
        }
    }


    qSort(m_titles);

    titleFile.close();
}
