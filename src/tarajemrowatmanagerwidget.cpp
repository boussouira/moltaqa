#include "tarajemrowatmanagerwidget.h"
#include "ui_tarajemrowatmanagerwidget.h"
#include "tarajemrowatmanager.h"
#include "utils.h"
#include "modelenums.h"

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

    m_manager = TarajemRowatManager::instance();
    loadModel();
    setupActions();

    enableEditWidgets(false);
}

TarajemRowatManagerWidget::~TarajemRowatManagerWidget()
{
    ML_DELETE_CHECK(m_webEditShoek);
    ML_DELETE_CHECK(m_webEditTalamid);
    ML_DELETE_CHECK(m_webEditTarjama);

    delete ui;
}

QString TarajemRowatManagerWidget::title()
{
    return tr("تراجم الرواة");
}

void TarajemRowatManagerWidget::beginEdit()
{
    ML_ASSERT(!m_webEditShoek
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
}

void TarajemRowatManagerWidget::save()
{
    saveCurrentRawi();
    ML_ASSERT(!m_editedRawiInfo.isEmpty());

    ML_BENCHMARK_START();

    ML_ASSERT2(m_manager->beginUpdate(), "TarajemRowatManagerWidget: Can't start update");

    foreach(RawiInfo *rawi, m_editedRawiInfo.values()) {
        m_manager->updateRawi(rawi);
    }

    m_manager->endUpdate();

    qDeleteAll(m_editedRawiInfo);
    m_editedRawiInfo.clear();
    m_currentRawi = 0;

    ML_BENCHMARK_ELAPSED("Save Rowat");

    setModified(false);
}

void TarajemRowatManagerWidget::loadModel()
{
    m_model = m_manager->getRowatModel();
    ui->treeView->setModel(m_model);
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

//    connect(ui->toolAdd, SIGNAL(clicked()), SLOT(newAuthor()));
//    connect(ui->toolRemove, SIGNAL(clicked()), SLOT(removeAuthor()));
}

void TarajemRowatManagerWidget::enableEditWidgets(bool enable)
{
    ui->tabWidget->setEnabled(enable);
}

void TarajemRowatManagerWidget::saveCurrentRawi()
{
    ML_ASSERT(m_currentRawi);

    m_currentRawi->name = ui->lineName->text();
    m_currentRawi->laqab = ui->lineLaqab->text();

    if(ui->checkUnknowBirth->isChecked()) {
        m_currentRawi->birthYear = m_currentRawi->UnknowYear;
        m_currentRawi->birthStr.clear();
    } else {
        m_currentRawi->birthYear = ui->spinBirth->value();
        m_currentRawi->birthStr = ui->lineBirthText->text();
    }

    if(ui->checkUnknowDeath->isChecked()){
        m_currentRawi->deathYear = m_currentRawi->UnknowYear;
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

RawiInfo *TarajemRowatManagerWidget::getRawiInfo(int rawiID)
{
    RawiInfo *rawi = m_editedRawiInfo.value(rawiID);
    if(!rawi) {
        rawi = m_manager->getRawiInfo(rawiID);
        if(rawi)
            rawi = rawi->clone();
    }

    return rawi;
}

void TarajemRowatManagerWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    int rawiID = index.data(ItemRole::authorIdRole).toInt();
    RawiInfo *rawi= getRawiInfo(rawiID);
    ML_ASSERT(rawi);

    ui->tabWidget->setCurrentIndex(0);

    saveCurrentRawi();

    if(m_currentRawi && !m_editedRawiInfo.contains(m_currentRawi->id))
        delete m_currentRawi;

    m_currentRawi = 0;

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
    ML_ASSERT(m_currentRawi);

    m_editedRawiInfo[m_currentRawi->id] = m_currentRawi;
    setModified(true);
}

void TarajemRowatManagerWidget::checkEditWebChange()
{
    ML_ASSERT(m_currentRawi);

    if(m_webEditShoek->pageModified()
            || m_webEditTalamid->pageModified()
            || m_webEditTarjama->pageModified()) {

        m_editedRawiInfo[m_currentRawi->id] = m_currentRawi;
        setModified(true);
    }
}

void TarajemRowatManagerWidget::birthDeathChanged()
{
    ui->spinBirth->setVisible(!ui->checkUnknowBirth->isChecked());
    ui->lineBirthText->setVisible(!ui->checkUnknowBirth->isChecked());

    ui->spinDeath->setVisible(!ui->checkUnknowDeath->isChecked());
    ui->lineDeathText->setVisible(!ui->checkUnknowDeath->isChecked());

    QSpinBox *spin = qobject_cast<QSpinBox*>(sender());
    if(spin && m_currentRawi) {
        if(spin == ui->spinBirth) {
            ui->lineBirthText->setText(Utils::hijriYear(spin->value()));
        } else if(spin == ui->spinDeath) {
            ui->lineDeathText->setText(Utils::hijriYear(spin->value()));
        }
    }
}
