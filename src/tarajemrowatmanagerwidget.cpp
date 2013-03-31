#include "tarajemrowatmanagerwidget.h"
#include "ui_tarajemrowatmanagerwidget.h"
#include "tarajemrowatmanager.h"
#include "librarymanager.h"
#include "utils.h"
#include "modelenums.h"
#include "modelutils.h"
#include "timeutils.h"
#include "modelviewfilter.h"
#include "editwebview.h"

#include <qstandarditemmodel.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

TarajemRowatManagerWidget::TarajemRowatManagerWidget(QWidget *parent) :
    ControlCenterWidget(parent),
    ui(new Ui::TarajemRowatManagerWidget),
    m_model(0),
    m_webEditShoek(0),
    m_webEditTalamid(0),
    m_webEditTarjama(0),
    m_currentRawi(0)
{
    ui->setupUi(this);

    m_manager = LibraryManager::instance()->rowatManager();
    m_filter = new ModelViewFilter(this);

    setupActions();
    enableEditWidgets(false);
}

TarajemRowatManagerWidget::~TarajemRowatManagerWidget()
{
    ml_delete_check(m_webEditShoek);
    ml_delete_check(m_webEditTalamid);
    ml_delete_check(m_webEditTarjama);

    delete ui;
}

QString TarajemRowatManagerWidget::title()
{
    return tr("تراجم الرواة");
}

void TarajemRowatManagerWidget::aboutToShow()
{
    ml_return_on_fail(!m_webEditShoek
              || !m_webEditTalamid
              || !m_webEditTarjama);

    m_webEditShoek = new EditWebView(this);
    m_webEditTalamid = new EditWebView(this);
    m_webEditTarjama = new EditWebView(this);

    QList<EditWebView*> edits;
    QList<QWidget*> widgets;
    edits << m_webEditShoek << m_webEditTalamid << m_webEditTarjama;
    widgets << ui->tabSheok << ui->tabTalamid << ui->tabTarejama;

    for(int i=0; i<edits.count(); i++) {
        QVBoxLayout *layout = new QVBoxLayout(widgets[i]);
        layout->addWidget(edits[i]);
    }

    if(Utils::Settings::contains("RowatManagerWidget/splitter"))
        ui->splitter->restoreState(Utils::Settings::get("RowatManagerWidget/splitter").toByteArray());
}

void TarajemRowatManagerWidget::aboutToHide()
{
    Utils::Settings::set("RowatManagerWidget/splitter", ui->splitter->saveState());
}

void TarajemRowatManagerWidget::save()
{
    saveCurrentRawi();
    ml_return_on_fail(m_editedRawiInfo.size());

    m_manager->transaction();

    foreach(RawiInfo::Ptr rawi, m_editedRawiInfo.values()) {
        m_manager->updateRawi(rawi);
    }

    m_manager->commit();
    m_manager->reloadModels();

    m_editedRawiInfo.clear();
    m_currentRawi.clear();

    setModified(false);
}

void TarajemRowatManagerWidget::loadModel()
{
    m_model = m_manager->getRowatModel();

    m_filter->setSourceModel(m_model);
    m_filter->setLineEdit(ui->lineFilter);
    m_filter->setTreeView(ui->treeView);
    m_filter->setup();
}

void TarajemRowatManagerWidget::setupActions()
{
    foreach(QLineEdit *edit, findChildren<QLineEdit *>()) {
        connect(edit, SIGNAL(textChanged(QString)), SLOT(infoChanged()));
    }

    connect(ui->tabWidget,
            SIGNAL(currentChanged(int)),
            SLOT(checkEditWebChange()));

    foreach(QSpinBox *spin, findChildren<QSpinBox *>()) {
        connect(spin, SIGNAL(valueChanged(int)), SLOT(infoChanged()));
        connect(spin, SIGNAL(valueChanged(int)), SLOT(birthDeathChanged()));
    }

    foreach(QCheckBox *check, findChildren<QCheckBox *>()) {
        connect(check, SIGNAL(stateChanged(int)), SLOT(infoChanged()));
        connect(check, SIGNAL(stateChanged(int)), SLOT(birthDeathChanged()));
    }

    connect(ui->toolAdd, SIGNAL(clicked()), SLOT(newRawi()));
    connect(ui->toolRemove, SIGNAL(clicked()), SLOT(removeRawi()));
}

void TarajemRowatManagerWidget::enableEditWidgets(bool enable)
{
    ui->tabWidget->setEnabled(enable);
}

void TarajemRowatManagerWidget::saveCurrentRawi()
{
    ui->tabWidget->setCurrentIndex(0);

    ml_return_on_fail(m_currentRawi);

    m_currentRawi->name = ui->lineName->text();
    m_currentRawi->laqab = ui->lineLaqab->text();

    if(ui->checkUnknowBirth->isChecked()) {
        m_currentRawi->birthYear = RawiInfo::UnknowYear;
        m_currentRawi->birthStr.clear();
    } else {
        m_currentRawi->birthYear = ui->spinBirth->value();
        m_currentRawi->birthStr = ui->lineBirthText->text();
    }

    if(ui->checkUnknowDeath->isChecked()){
        m_currentRawi->deathYear = RawiInfo::UnknowYear;
        m_currentRawi->deathStr.clear();
    } else {
        m_currentRawi->deathYear = ui->spinDeath->value();
        m_currentRawi->deathStr = ui->lineDeathText->text();
    }

    m_currentRawi->tabaqa = ui->lineTabaqa->text();
    m_currentRawi->rowat = ui->lineRawaLaho->text();
    m_currentRawi->rotba_hafed = ui->lineIbnHajar->text();
    m_currentRawi->rotba_zahabi = ui->lineAlDahabi->text();

    m_currentRawi->sheok = m_webEditShoek->editorText();
    m_currentRawi->talamid = m_webEditTalamid->editorText();
    m_currentRawi->tarejama = m_webEditTarjama->editorText();

}

RawiInfo::Ptr TarajemRowatManagerWidget::getRawiInfo(int rawiID)
{
    RawiInfo::Ptr rawi = m_editedRawiInfo.value(rawiID);
    if(!rawi) {
        rawi = m_manager->getRawiInfo(rawiID);
        if(rawi)
            rawi = RawiInfo::Ptr(rawi->clone());
    }

    return rawi;
}

void TarajemRowatManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    int rawiID = index.data(ItemRole::authorIdRole).toInt();
    RawiInfo::Ptr rawi= getRawiInfo(rawiID);
    ml_return_on_fail(rawi);

    saveCurrentRawi();

    m_currentRawi.clear();

    ui->lineName->setText(rawi->name);
    ui->lineLaqab->setText(rawi->laqab);

    ui->checkUnknowBirth->setChecked(rawi->unknowBirth());
    ui->spinBirth->setValue(rawi->birthYear);
    ui->lineBirthText->setText(rawi->birthStr);

    ui->checkUnknowDeath->setChecked(rawi->unknowDeath());
    ui->spinDeath->setValue(rawi->deathYear);
    ui->lineDeathText->setText(rawi->deathStr);

    ui->lineTabaqa->setText(rawi->tabaqa);
    ui->lineRawaLaho->setText(rawi->rowat);
    ui->lineIbnHajar->setText(rawi->rotba_hafed);
    ui->lineAlDahabi->setText(rawi->rotba_zahabi);

    m_webEditShoek->setEditorText(rawi->sheok);
    m_webEditTalamid->setEditorText(rawi->talamid);
    m_webEditTarjama->setEditorText(rawi->tarejama);

    enableEditWidgets(true);

    m_currentRawi = rawi;
}

void TarajemRowatManagerWidget::infoChanged()
{
    ml_return_on_fail(m_currentRawi);

    m_editedRawiInfo[m_currentRawi->id] = m_currentRawi;
    setModified(true);
}

void TarajemRowatManagerWidget::checkEditWebChange()
{
    ml_return_on_fail(m_webEditShoek
              && m_webEditTalamid
              && m_webEditTarjama);

    if(m_webEditShoek->pageModified()
            || m_webEditTalamid->pageModified()
            || m_webEditTarjama->pageModified()) {

        infoChanged();
    }
}

void TarajemRowatManagerWidget::birthDeathChanged()
{
    ui->spinBirth->setVisible(!ui->checkUnknowBirth->isChecked());
    ui->lineBirthText->setVisible(!ui->checkUnknowBirth->isChecked());

    ui->spinDeath->setVisible(!ui->checkUnknowDeath->isChecked());
    ui->lineDeathText->setVisible(!ui->checkUnknowDeath->isChecked());

    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    ml_return_on_fail(spin);

    if(spin->value() == RawiInfo::UnknowYear)
        spin->setValue(0);

    ml_return_on_fail(m_currentRawi);

    if(spin == ui->spinBirth) {
        ui->lineBirthText->setText(Utils::Time::hijriYear(spin->value()));
    } else if(spin == ui->spinDeath) {
        ui->lineDeathText->setText(Utils::Time::hijriYear(spin->value()));
    }
}

void TarajemRowatManagerWidget::newRawi()
{
    QString name = QInputDialog::getText(this,
                                         tr("اضافة راوي"),
                                         tr("اسم الراوي:"));
    if(name.size()) {
        RawiInfo::Ptr rawi = RawiInfo::Ptr(new RawiInfo());
        rawi->name = name;

        m_manager->addRawi(rawi);

        QStandardItem *authItem = new QStandardItem();
        authItem->setText(name);
        authItem->setData(rawi->id, ItemRole::authorIdRole);

        m_model->appendRow(authItem);

        QModelIndex index = m_model->indexFromItem(authItem);
        Utils::Model::selectIndex(ui->treeView, index);
        on_treeView_doubleClicked(index);

        m_currentRawi = rawi;

        infoChanged();
        saveCurrentRawi();
    }
}

void TarajemRowatManagerWidget::removeRawi()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()) {
        if(QMessageBox::question(this,
                                 tr("حذف الراوي"),
                                 tr("هل انت متأكد من انك تريد حذف الراوي '%1'؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {

            int rawiID = index.data(ItemRole::authorIdRole).toInt();
            RawiInfo::Ptr rawi= getRawiInfo(rawiID);
            ml_return_on_fail(rawi);

            m_manager->removeRawi(rawi->id);

            m_model->takeRow(index.row());
        }
    } else {
        QMessageBox::warning(this,
                             tr("حذف الراوي"),
                             tr("لم تقم باختيار اي الراوي"));
    }
}
