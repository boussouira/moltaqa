#ifndef QURANTEXTMODEL_H
#define QURANTEXTMODEL_H

#include <QObject>
#include "qurantextbase.h"
#include "pageinfo.h"
#include "ktext.h"

class QuranTextModel : public QuranTextBase, public KText
{
public:
    QuranTextModel(QObject *parent);
    /**
      @brief Get formated Quran text of the given PageInfo from the database.
      @param pPageInfo      Page information(SORA, AYA and page...)
      @return Formatted text.
      @see KText::text()
      */
    QString getQuranPage(PageInfo *pPageInfo);
};

#endif // QURANTEXTMODEL_H
