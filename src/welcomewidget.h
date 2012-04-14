#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "abstarctview.h"
#include <qstandarditemmodel.h>

class BookListManager;
class LibraryManager;
class FavouritesManager;
class LibraryBookManager;
class ModelViewFilter;

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

    void aboutToShow();
    void aboutToHide();

protected:
    void loadSettings();
    void saveSettings();

protected slots:
    void bookListModel();
    void favouritesModel();
    void lastReadBooksModel();
    void itemClicked(QModelIndex index);
    void lastOpenedItemClicked(QModelIndex index);

signals:
    void bookSelected(int bookID);

private:
    BookListManager *m_bookListManager;
    FavouritesManager *m_favouritesManager;
    LibraryBookManager *m_bookManager;
    QStandardItemModel *m_bookListModel;
    QStandardItemModel *m_favouritesModel;
    QStandardItemModel *m_lastReadedModel;
    ModelViewFilter *m_bookListFilter;
    ModelViewFilter *m_favouritesListFilter;
    Ui::WelcomeWidget *ui;
};

#endif // WELCOMEWIDGET_H
