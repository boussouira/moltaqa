#include "shamelaimportinfo.h"
#include <qdebug.h>

ShamelaBookInfo::ShamelaBookInfo(int bid, QString bname, QString binfo, int barchive, int bcat, int bauth, QString bauthName):
    id(bid),
    name(bname),
    info(binfo),
    archive(barchive),
    cat(bcat),
    authorID(bauth),
    authName(bauthName)
{
}

ShamelaBookInfo::~ShamelaBookInfo()
{
}

void ShamelaBookInfo::genInfo()
{
    mainTable = (archive) ? QString("b%1").arg(id) : "book";
    tocTable = (archive) ? QString("t%1").arg(id) : "title";
}

void ShamelaBookInfo::genInfo(ShamelaInfo *info)
{
    genInfo();

    path = info->buildFilePath(QString::number(id), archive);
}
