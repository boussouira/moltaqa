#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <qstackedwidget.h>
#include <qstack.h>

class MainWindow;
class AbstarctView;
class QMenu;

class ViewManager : public QStackedWidget
{
    Q_OBJECT
public:
    ViewManager(QWidget *parent = 0);

    void addView(AbstarctView *view, bool selectable=true);
    void removeView(AbstarctView *view);
    void setCurrentView(int index);
    void setCurrentView(AbstarctView *view);
    void setDefautView(AbstarctView *view);

    void setMenu(QMenu *menu);
    void setupActions();

protected:
    void removeViewFromStack(AbstarctView *view);
    void addViewToStack(AbstarctView *view);

protected slots:
    void changeWindow();

public slots:
    void hideView();

protected:
    MainWindow *m_mainWindow;
    QMenu *m_menu;
    QStack<AbstarctView*> m_viewDisplay;
    AbstarctView *m_defautView;
};

#endif // VIEWMANAGER_H
