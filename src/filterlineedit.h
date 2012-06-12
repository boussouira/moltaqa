#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include "fancylineedit.h"

class FilterLineEdit : public FancyLineEdit
{
    Q_OBJECT
public:
    explicit FilterLineEdit(QWidget *parent = 0);

    void setFilterMenu(QMenu *menu);

signals:
    void filterChanged(const QString &);

private slots:
    void slotTextChanged();

private:
    QString m_lastFilterText;
};

#endif // FILTERLINEEDIT_H
