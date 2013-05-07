#ifndef ABSTARCTVIEW_H
#define ABSTARCTVIEW_H

#include <qevent.h>
#include <qwidget.h>

class FilterLineEdit;
class QToolBar;
class QKeyEvent;

class AbstarctView : public QWidget
{
    Q_OBJECT
public:
    AbstarctView(QWidget *parent = 0);
    ~AbstarctView();

    virtual QList<QToolBar*> toolBars();
    virtual QList<QAction*> navigationActions();
    virtual void updateToolBars();
    virtual void updateActions();

    virtual QString title()=0;

    virtual void aboutToShow();
    virtual void aboutToHide();

    bool isSelectable();
    void setSelectable(bool selectebale);

    static QAction *actionSeparator(QObject *parent=0);

    virtual QString viewLink();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void hideMe();
    void showMe();

protected:
    QList<QToolBar*> m_toolBars;
    QList<QAction*> m_navActions;
    bool m_selectable;
    bool m_crtlKey;
};

#endif // ABSTARCTVIEW_H
