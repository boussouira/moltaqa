#ifndef WINDOWSVIEW_H
#define WINDOWSVIEW_H

#include <QDialog>

namespace Ui {
class WindowsView;
}

class AbstarctView;
class QListWidget;
class QListWidgetItem;
class QModelIndex;

class WindowsView : public QDialog
{
    Q_OBJECT

public:
    WindowsView(QWidget *parent = 0);
    ~WindowsView();

    void addView(AbstarctView *view, int index);
    void selectCurrentView();
    void selectNextItem(int summand);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected slots:
    void currentViewChanged(const QModelIndex &current, const QModelIndex &previous);
    void itemClicked(QListWidgetItem *item);

signals:
    void selectView(QString title);

private:
    QListWidget *m_listWidget;
};

#endif // WINDOWSVIEW_H
