#include "textbookreader.h"

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
}

TextBookReader::~TextBookReader()
{
}

void TextBookReader::getTitles()
{
    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("testRead(): file.open(): %d", titleFile.getZipError());
            return;
        }
    }

    QDomDocument doc;
    if(!doc.setContent(&titleFile)) {
        qDebug("Error");
        titleFile.close();
        return;
    }

    QDomElement root = doc.documentElement();
    QDomNode itemNode = root.firstChild();

    while(!itemNode.isNull()) {
        m_titles.append(itemNode.attributes().namedItem("pageID").nodeValue().toInt());

        itemNode = itemNode.nextSibling();
    }

    qSort(m_titles);
}

void TextBookReader::connected()
{
    m_bookQuery = QSqlQuery(m_bookDB);

    AbstractBookReader::connected();
}
