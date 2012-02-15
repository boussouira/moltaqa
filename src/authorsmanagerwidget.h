#ifndef AUTHORSMANAGERWIDGET_H
#define AUTHORSMANAGERWIDGET_H

#include "controlcenterwidget.h"
#include <qhash.h>

namespace Ui {
class AuthorsManagerWidget;
}

class QStandardItemModel;
class AuthorInfo;
class QModelIndex;
class AuthorsManager;

class AuthorsManagerWidget : public ControlCenterWidget
{
    Q_OBJECT
    
public:
    AuthorsManagerWidget(QWidget *parent = 0);
    ~AuthorsManagerWidget();
    
    QString title();

    void beginEdit();
    void save();

protected:
    void setupActions();
    void enableEditWidgets(bool enable);
    void loadModel();
    void saveCurrentAuthor();
    AuthorInfo *getAuthorInfo(int authorID);

private slots:
    void infoChanged();
    void birthDeathChanged();
    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::AuthorsManagerWidget *ui;
    QStandardItemModel *m_model;
    AuthorsManager *m_authorsManager;
    QHash<int, AuthorInfo*> m_editedAuthInfo;
    AuthorInfo* m_currentAuthor;
};

#endif // AUTHORSMANAGERWIDGET_H
