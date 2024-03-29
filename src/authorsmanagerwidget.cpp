#include "authorsmanagerwidget.h"
#include "ui_authorsmanagerwidget.h"
#include "authorsmanager.h"
#include "editwebview.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "modelviewfilter.h"
#include "timeutils.h"
#include "utils.h"

#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qstandarditemmodel.h>

AuthorsManagerWidget::AuthorsManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::AuthorsManagerWidget),
    m_model(0),
    m_webEdit(0)
{
    ui->setupUi(this);

    m_authorsManager = LibraryManager::instance()->authorsManager();
    m_filter = new ModelViewFilter(this);

    setupActions();
    enableEditWidgets(false);
}

AuthorsManagerWidget::~AuthorsManagerWidget()
{
    ml_delete_check(m_model);

    delete ui;
}

QString AuthorsManagerWidget::title()
{
    return tr("المؤلفين");
}

void AuthorsManagerWidget::setupActions()
{
    foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
        connect(edit, SIGNAL(textChanged(QString)), SLOT(infoChanged()));
    }

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(checkEditWebChange()));

    foreach(QSpinBox *spin, findChildren<QSpinBox *>()) {
        connect(spin, SIGNAL(valueChanged(int)), SLOT(infoChanged()));
        connect(spin, SIGNAL(valueChanged(int)), SLOT(birthDeathChanged()));
    }

    foreach(QCheckBox *check, findChildren<QCheckBox *>()) {
        connect(check, SIGNAL(stateChanged(int)), SLOT(infoChanged()));
        connect(check, SIGNAL(stateChanged(int)), SLOT(birthDeathChanged()));
    }

    connect(ui->toolAdd, SIGNAL(clicked()), SLOT(newAuthor()));
    connect(ui->toolRemove, SIGNAL(clicked()), SLOT(removeAuthor()));
}

void AuthorsManagerWidget::loadModel()
{
    m_model = m_authorsManager->authorsModel();

    m_filter->setSourceModel(m_model);
    m_filter->setLineEdit(ui->lineFilter);
    m_filter->setTreeView(ui->treeView);
    m_filter->setup();
}

void AuthorsManagerWidget::infoChanged()
{
    if(m_currentAuthor) {
        m_editedAuthInfo[m_currentAuthor->id] = m_currentAuthor;

        setModified(true);
    }
}

void AuthorsManagerWidget::checkEditWebChange()
{
    ml_return_on_fail(m_webEdit);

    if(m_webEdit->pageModified())
        infoChanged();
}

void AuthorsManagerWidget::birthDeathChanged()
{
    if(ui->checkALive->isChecked()) {
        ui->lineDeathText->setVisible(false);
        ui->spinDeath->setVisible(false);
        ui->checkUnknowDeath->setVisible(false);
    } else if(ui->checkUnknowDeath->isChecked()) {
        ui->lineDeathText->setVisible(false);
        ui->spinDeath->setVisible(false);
        ui->checkALive->setVisible(false);
    } else {
        ui->lineDeathText->setVisible(true);
        ui->spinDeath->setVisible(true);
        ui->checkALive->setVisible(true);
        ui->checkUnknowDeath->setVisible(true);
    }

    ui->spinBirth->setVisible(!ui->checkUnknowBirth->isChecked());
    ui->lineBirthText->setVisible(!ui->checkUnknowBirth->isChecked());

    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if(spin && m_currentAuthor) {
        if(spin == ui->spinBirth) {
            ui->lineBirthText->setText(Utils::Time::hijriYear(spin->value()));
        } else if(spin == ui->spinDeath) {
            ui->lineDeathText->setText(Utils::Time::hijriYear(spin->value()));
        }
    }
}

void AuthorsManagerWidget::newAuthor()
{
    QString name = QInputDialog::getText(this,
                                         tr("اضافة مؤلف"),
                                         tr("اسم المؤلف:"));
    if(name.size()) {
        AuthorInfo::Ptr auth(new AuthorInfo());
        auth->name = name;
        auth->fullName = name;
        auth->unknowBirth = true;
        auth->unknowDeath = true;

        m_authorsManager->addAuthor(auth);

        QStandardItem *authItem = new QStandardItem();
        authItem->setText(name);
        authItem->setData(auth->id, ItemRole::authorIdRole);

        m_model->appendRow(authItem);

        QModelIndex index = m_model->indexFromItem(authItem);
        Utils::Model::selectIndex(ui->treeView, index);
        on_treeView_doubleClicked(index);
    }
}

void AuthorsManagerWidget::removeAuthor()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        if(QMessageBox::question(this,
                                 tr("حذف مؤلف"),
                                 tr("هل انت متأكد من انك تريد حذف '%1' من مجموعة المؤلفين؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
            int autID = index.data(ItemRole::authorIdRole).toInt();
            m_model->removeRow(index.row(), index.parent());

            m_authorsManager->removeAuthor(autID);
        }
    } else {
        QMessageBox::warning(this,
                             tr("حذف مؤلف"),
                             tr("لم تقم باختيار اي مؤلف"));
    }
}

void AuthorsManagerWidget::saveCurrentAuthor()
{
    ui->tabWidget->setCurrentIndex(0);

    if(m_currentAuthor) {
        m_currentAuthor->name = ui->lineName->text();
        m_currentAuthor->fullName = ui->lineFullName->text();
        m_currentAuthor->info = m_webEdit->editorText();

        if(!ui->checkUnknowBirth->isChecked()) {
            m_currentAuthor->birthYear = ui->spinBirth->value();
            m_currentAuthor->birthStr = ui->lineBirthText->text();
            m_currentAuthor->unknowBirth = false;
        } else {
            m_currentAuthor->unknowBirth = true;
        }

        m_currentAuthor->isALive= false;
        m_currentAuthor->unknowDeath = false;

        if(ui->checkALive->isChecked()){
            m_currentAuthor->isALive= true;
        } else if(!ui->checkUnknowDeath->isChecked()) {
            m_currentAuthor->deathYear = ui->spinDeath->value();
            m_currentAuthor->deathStr = ui->lineDeathText->text();
        } else {
            m_currentAuthor->unknowDeath = true;
        }
    }
}

AuthorInfo::Ptr AuthorsManagerWidget::getAuthorInfo(int authorID)
{
    AuthorInfo::Ptr auth = m_editedAuthInfo.value(authorID);
    if(!auth) {
        auth = m_authorsManager->getAuthorInfo(authorID);
        if(auth)
            auth = AuthorInfo::Ptr(auth->clone());
    }

    return auth;
}

void AuthorsManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    ml_return_on_fail(m_webEdit);

    int authorID = index.data(ItemRole::authorIdRole).toInt();
    AuthorInfo::Ptr auth = getAuthorInfo(authorID);
    if(auth) {
        saveCurrentAuthor();

        m_currentAuthor.clear();

        ui->lineName->setText(auth->name);
        ui->lineFullName->setText(auth->fullName);
        m_webEdit->setEditorText(auth->info);

        ui->checkUnknowBirth->setChecked(auth->unknowBirth);
        ui->spinBirth->setValue(auth->birthYear);
        ui->lineBirthText->setText(auth->birthStr);

        ui->checkALive->setChecked(auth->isALive);
        ui->checkUnknowDeath->setChecked(auth->unknowDeath);
        ui->spinDeath->setValue(auth->deathYear);
        ui->lineDeathText->setText(auth->deathStr);

        enableEditWidgets(true);

        m_currentAuthor = auth;
    }
}

void AuthorsManagerWidget::enableEditWidgets(bool enable)
{
    ui->tabAuthor->setEnabled(enable);
}

void AuthorsManagerWidget::save()
{
    saveCurrentAuthor();

    ml_return_on_fail(m_editedAuthInfo.size());

    m_authorsManager->transaction();

    foreach(AuthorInfo::Ptr auth, m_editedAuthInfo.values()) {
        m_authorsManager->updateAuthor(auth);
    }

    m_authorsManager->commit();
    m_authorsManager->reloadModels();

    m_editedAuthInfo.clear();
    m_currentAuthor.clear();

    setModified(false);
}

void AuthorsManagerWidget::aboutToShow()
{
    ml_return_on_fail(!m_webEdit);

    m_webEdit = new EditWebView(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->tabTarjama);
    layout->addWidget(m_webEdit);

    if(Utils::Settings::contains("AuthorsManagerWidget/splitter"))
        ui->splitter->restoreState(Utils::Settings::get("AuthorsManagerWidget/splitter").toByteArray());
}

void AuthorsManagerWidget::aboutToHide()
{
    Utils::Settings::set("AuthorsManagerWidget/splitter", ui->splitter->saveState());
}
