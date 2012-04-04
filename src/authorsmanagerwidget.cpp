#include "authorsmanagerwidget.h"
#include "ui_authorsmanagerwidget.h"
#include "authorsmanager.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "utils.h"
#include "modelutils.h"
#include <qstandarditemmodel.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

AuthorsManagerWidget::AuthorsManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::AuthorsManagerWidget),
    m_model(0),
    m_currentAuthor(0)
{
    ui->setupUi(this);

    m_authorsManager = AuthorsManager::instance();

    loadModel();
    setupActions();
    enableEditWidgets(false);
}

AuthorsManagerWidget::~AuthorsManagerWidget()
{
    ML_DELETE_CHECK(m_model);

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

    foreach(QTextEdit *edit, findChildren<QTextEdit *>()) {
        connect(edit, SIGNAL(textChanged()), SLOT(infoChanged()));
    }

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
    ui->treeView->setModel(m_model);
}

void AuthorsManagerWidget::infoChanged()
{
    if(m_currentAuthor) {
        m_editedAuthInfo[m_currentAuthor->id] = m_currentAuthor;

        setModified(true);
    }
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
            ui->lineBirthText->setText(Utils::hijriYear(spin->value()));
        } else if(spin == ui->spinDeath) {
            ui->lineDeathText->setText(Utils::hijriYear(spin->value()));
        }
    }
}

void AuthorsManagerWidget::newAuthor()
{
    QString name = QInputDialog::getText(this,
                                         tr("اضافة مؤلف"),
                                         tr("اسم المؤلف:"));
    if(!name.isEmpty()) {
        AuthorInfo *auth = new AuthorInfo();
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
        Utils::selectIndex(ui->treeView, index);
        on_treeView_doubleClicked(index);
    }
}

void AuthorsManagerWidget::removeAuthor()
{
    QModelIndex index = Utils::selectedIndex(ui->treeView);
    if(index.isValid()) {
        if(QMessageBox::question(this,
                                 tr("حذف مؤلف"),
                                 tr("هل انت متأكد من انك تريد حذف '%1' من مجموعة المؤلفين؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
            int autID = index.data(ItemRole::authorIdRole).toInt();
            m_model->removeRow(index.row(), index.parent());

            m_deletedAuth.append(autID);
        }
    } else {
        QMessageBox::warning(this,
                             tr("حذف مؤلف"),
                             tr("لم تقم باختيار اي مؤلف"));
    }
}

void AuthorsManagerWidget::saveCurrentAuthor()
{
    if(m_currentAuthor) {
        m_currentAuthor->name = ui->lineName->text();
        m_currentAuthor->fullName = ui->lineFullName->text();
        m_currentAuthor->info = ui->textEdit->toPlainText();

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

AuthorInfo *AuthorsManagerWidget::getAuthorInfo(int authorID)
{
    AuthorInfo *auth = m_editedAuthInfo.value(authorID);
    if(!auth) {
        auth = m_authorsManager->getAuthorInfo(authorID);
        if(auth)
            auth = auth->clone();
    }

    return auth;
}

void AuthorsManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    int authorID = index.data(ItemRole::authorIdRole).toInt();
    AuthorInfo *auth = getAuthorInfo(authorID);
    if(auth) {
        saveCurrentAuthor();

        if(m_currentAuthor && !m_editedAuthInfo.contains(m_currentAuthor->id))
            delete m_currentAuthor;

        m_currentAuthor = 0;

        ui->lineName->setText(auth->name);
        ui->lineFullName->setText(auth->fullName);
        ui->textEdit->setPlainText(auth->info);

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
    ui->groupBox->setEnabled(enable);
}

void AuthorsManagerWidget::save()
{
    saveCurrentAuthor();

    ML_ASSERT(!m_editedAuthInfo.isEmpty() || !m_deletedAuth.isEmpty());

    m_authorsManager->transaction();

    foreach(AuthorInfo *auth, m_editedAuthInfo.values()) {
        if(m_deletedAuth.isEmpty() || !m_deletedAuth.contains(auth->id))
            m_authorsManager->updateAuthor(auth);
    }

    foreach (int authorID, m_deletedAuth) {
        m_authorsManager->removeAuthor(authorID);
    }

    m_authorsManager->commit();

    m_editedAuthInfo.clear();
    m_deletedAuth.clear();
    m_currentAuthor = 0;

    setModified(false);
}

void AuthorsManagerWidget::beginEdit()
{
}
