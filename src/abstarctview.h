#ifndef ABSTARCTVIEW_H
#define ABSTARCTVIEW_H

#include <qwidget.h>
#include <qtoolbar.h>

class ViewConfig
{
    ViewConfig(){}

};

class AbstarctView : public QWidget
{
    Q_OBJECT
public:
    AbstarctView(QWidget *parent = 0);

    virtual QList<QToolBar*> toolBars();
    virtual void showToolBars();
    virtual void hideToolBars();
    virtual void hideMenu();
    virtual void showMenu();
    virtual QString title()=0;
    bool isSelectable();
    void setSelectable(bool selectebale);

protected:
    QList<QToolBar *> m_toolBars;
    bool m_selectable;
};

#endif // ABSTARCTVIEW_H
