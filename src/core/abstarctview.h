#ifndef ABSTARCTVIEW_H
#define ABSTARCTVIEW_H

#include <qevent.h>
#include <qwidget.h>

class FilterLineEdit;
class WebViewSearcher;
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
    virtual WebViewSearcher *searcher();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected slots:
    void searchInPage();
    void searchNext();
    void searchPrev();
    void updateSearchNavigation();

signals:
    void hideMe();
    void showMe();

protected:
    QList<QToolBar*> m_toolBars;
    QList<QAction*> m_navActions;
    QToolBar *m_toolBarSearch;
    FilterLineEdit *m_searchEdit;
    QAction *m_searchPrevAction;
    QAction *m_searchNextAction;
    bool m_selectable;
    bool m_crtlKey;
};

#endif // ABSTARCTVIEW_H
