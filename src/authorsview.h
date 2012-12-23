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
    QString viewLink();
    void aboutToShow();
    void aboutToHide();

    bool openAuthorInfo(int authorID);

protected:
    int addTab(QString tabText=QString());
    void setCurrentAuth(AuthorInfo::Ptr info);
    void setCurrentTabHtml(QString title, QString html);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void lastTabClosed();

protected:
    Ui::AuthorsView *ui;
    AuthorsManager *m_manager;
    QStandardItemModel *m_model;
    ModelViewFilter *m_filter;
    LibraryBookManager *m_bookManager;
    AuthorInfo::Ptr m_currentAuthor;
};

#endif // AUTHORSVIEW_H
