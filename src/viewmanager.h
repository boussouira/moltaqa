#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <qstackedwidget.h>

class MainWindow;
class AbstarctView;
class QMenu;

class ViewManager : public QStackedWidget
{
    Q_OBJECT
public:
    ViewManager(QWidget *parent = 0);

    void addView(AbstarctView *view);
    void removeView(AbstarctView *view);
    void setCurrentView(int index);

    void setMenu(QMenu *menu);
    void setupActions();

protected slots:
    void changeWindow();

protected:
    MainWindow *m_mainWindow;
    QMenu *m_menu;
};

#endif // VIEWMANAGER_H
