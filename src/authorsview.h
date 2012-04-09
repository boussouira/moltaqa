#ifndef AUTHORSVIEW_H
#define AUTHORSVIEW_H

#include "abstarctview.h"
#include "authorsmanager.h"
#include <qstandarditemmodel.h>

namespace Ui {
class AuthorsView;
}

class ModelViewFilter;
class LibraryBookManager;

class AuthorsView : public AbstarctView
{
    Q_OBJECT
    
public:
    AuthorsView(QWidget *parent = 0);
    ~AuthorsView();

    static AuthorsView *instance();

    QString title();
    void aboutToShow();

    void openAuthorInfo(int authorID);

protected:
    int addTab(QString tabText=QString());
    void setCurrentAuth(AuthorInfoPtr info);
    void setCurrentTabHtml(QString title, QString html);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);

protected:
    Ui::AuthorsView *ui;
    AuthorsManager *m_manager;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    LibraryBookManager *m_bookManager;
};

#endif // AUTHORSVIEW_H
