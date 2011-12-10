#include "textbookreader.h"

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
}

TextBookReader::~TextBookReader()
{
}

void TextBookReader::connected()
{
    m_bookQuery = QSqlQuery(m_bookDB);

    AbstractBookReader::connected();
}
