#include "richbookreader.h"
#include "textformatter.h"

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
}

RichBookReader::~RichBookReader()
{
    if(m_textFormat)
        delete m_textFormat;
}

bool RichBookReader::needFastIndexLoad()
{
    return true;
}

QString RichBookReader::text()
{
    Q_ASSERT(m_textFormat);

    return m_textFormat->getText();
}

QAbstractItemModel * RichBookReader::topIndexModel()
{
    return new QStandardItemModel();
}
