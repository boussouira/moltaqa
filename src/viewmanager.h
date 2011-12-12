#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <qstackedwidget.h>
#include <qset.h>

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

    void setWindowsMenu(QMenu *menu);
    void setNavigationMenu(QMenu *menu);
    void setupWindowsActions();

protected slots:
    void changeWindow();

public slots:
    void hideView();
    void showView();

protected:
    MainWindow *m_mainWindow;
    QMenu *m_windowsMenu;
    QMenu *m_navigationsMenu;
    QSet<AbstarctView*> m_viewDisplay;
    AbstarctView *m_defautView;
    AbstarctView *m_currentView;
};

#endif // VIEWMANAGER_H
