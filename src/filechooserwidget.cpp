#include "filechooserwidget.h"
#include "utils.h"

#include <qboxlayout.h>
#include <qtoolbutton.h>
#include <qfiledialog.h>

FileChooserWidget::FileChooserWidget(QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel(this);
    m_edit = new QLineEdit(this);
    m_button = new QToolButton(this);

    m_label->setText(QString::fromUtf8("مجلد وضع الملفات:"));
    m_button->setText("...");
    m_edit->setReadOnly(true);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_label);
    layout->addWidget(m_edit, 1);
    layout->addWidget(m_button);

    setLayout(layout);

    connect(m_button, SIGNAL(clicked()), SLOT(chooseFolder()));
}

void FileChooserWidget::chooseFolder()
{
    QString lastPath = m_settingName.isEmpty()
            ? m_edit->text()
            : Utils::Settings::get("SavedPath/"+m_settingName, m_edit->text()).toString();

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                        lastPath,
                                                        QFileDialog::ShowDirsOnly);
    if(dirPath.size()) {
        m_edit->setText(QDir::toNativeSeparators(dirPath));
        Utils::Settings::set("SavedPath/"+m_settingName, dirPath);
    }
}
