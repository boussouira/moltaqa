#ifndef FILECHOOSERWIDGET_H
#define FILECHOOSERWIDGET_H

#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>

class FileChooserWidget : public QWidget
{
    Q_OBJECT
public:
    FileChooserWidget(QWidget *parent = 0);

    void setLabelText(const QString &text) { m_label->setText(text); }
    void setSettingName(const QString &name) { m_settingName = name;}

    QString getPath() { return m_edit->text(); }
    QLineEdit *lineEdit() { return m_edit; }

protected slots:
    void chooseFolder();

protected:
    class QToolButton *m_button;
    QLabel *m_label;
    QLineEdit *m_edit;
    QString m_settingName;
};

#endif // FILECHOOSERWIDGET_H
