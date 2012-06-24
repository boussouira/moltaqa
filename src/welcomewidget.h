#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "abstarctview.h"

class LibraryBookManager;

namespace Ui {
    class WelcomeWidget;
}

class WelcomeWidget : public AbstarctView
{
    Q_OBJECT

public:
    WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

    QString title();

protected:
    void loadSettings();
    void saveSettings();
    void setupHTML();

public slots:
    void open(QString vid);

protected slots:
    void jsCleared();
    void showStatistics();

signals:
    void bookSelected(int bookID);

private:
    LibraryBookManager *m_bookManager;
    Ui::WelcomeWidget *ui;
};

#endif // WELCOMEWIDGET_H
