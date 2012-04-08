#ifndef AUTHORSMANAGERWIDGET_H
#define AUTHORSMANAGERWIDGET_H

#include "controlcenterwidget.h"
#include "authorsmanager.h"
#include <qhash.h>

namespace Ui {
class AuthorsManagerWidget;
}

class QStandardItemModel;
class QModelIndex;
class AuthorsManager;
class EditWebView;

class AuthorsManagerWidget : public ControlCenterWidget
{
    Q_OBJECT
    
public:
    AuthorsManagerWidget(QWidget *parent = 0);
    ~AuthorsManagerWidget();
    
    QString title();
    void save();

    void aboutToShow();

protected:
    void setupActions();
    void enableEditWidgets(bool enable);
    void loadModel();
    void saveCurrentAuthor();
    AuthorInfoPtr getAuthorInfo(int authorID);

private slots:
    void infoChanged();
    void checkEditWebChange();
    void birthDeathChanged();
    void newAuthor();
    void removeAuthor();
    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::AuthorsManagerWidget *ui;
    QStandardItemModel *m_model;
    AuthorsManager *m_authorsManager;
    EditWebView *m_webEdit;
    QHash<int, AuthorInfoPtr> m_editedAuthInfo;
    AuthorInfoPtr m_currentAuthor;
};

#endif // AUTHORSMANAGERWIDGET_H
