#include "shamelaimportinfo.h"
#include <qdebug.h>

ShamelaBookInfo::ShamelaBookInfo(int bid, QString bname, QString binfo, int barchive, int bcat, int bauth, QString bauthName, QString btafessirName) :
    id(bid),
    name(bname),
    info(binfo),
    archive(barchive),
    cat(bcat),
    authorID(bauth),
    authName(bauthName),
    tafessirName(btafessirName)
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

void ShamelaBookInfo::genInfo(ShamelaInfo *shaInfo)
{
    genInfo();

    path = shaInfo->buildFilePath(QString::number(id), archive);
}
