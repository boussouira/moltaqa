#ifndef CHECKABLEMESSAGEBOX_H
#define CHECKABLEMESSAGEBOX_H

#include <QDialogButtonBox>
#include <QMessageBox>

class CheckableMessageBoxPrivate;

class CheckableMessageBox : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QPixmap iconPixmap READ iconPixmap WRITE setIconPixmap)
    Q_PROPERTY(bool isChecked READ isChecked WRITE setChecked)
    Q_PROPERTY(QString checkBoxText READ checkBoxText WRITE setCheckBoxText)
    Q_PROPERTY(QDialogButtonBox::StandardButtons buttons READ standardButtons WRITE setStandardButtons)
    Q_PROPERTY(QDialogButtonBox::StandardButton defaultButton READ defaultButton WRITE setDefaultButton)

public:
    explicit CheckableMessageBox(QWidget *parent);
    virtual ~CheckableMessageBox();

    static QDialogButtonBox::StandardButton
        question(QWidget *parent,
                 const QString &title,
                 const QString &question,
                 const QString &settingsKey,
                 QDialogButtonBox::StandardButton returnButtonIfNotShown,
                 const QString &checkBoxText = QString(),
                 QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Yes|QDialogButtonBox::No,
                 QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::No);

    static QDialogButtonBox::StandardButton
        information(QWidget *parent,
                const QString &title,
                const QString &messageText,
                const QString &settingsKey,
                QDialogButtonBox::StandardButton returnButtonIfNotShown = QDialogButtonBox::Ok,
                const QString &checkBoxText = QString(),
                QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok,
                QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::Ok);

    static QDialogButtonBox::StandardButton
        warning(QWidget *parent,
                const QString &title,
                const QString &messageText,
                const QString &settingsKey,
                QDialogButtonBox::StandardButton returnButtonIfNotShown = QDialogButtonBox::Ok,
                const QString &checkBoxText = QString(),
                QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok,
                QDialogButtonBox::StandardButton defaultButton = QDialogButtonBox::Ok);

    QString text() const;
    void setText(const QString &);

    bool isChecked() const;
    void setChecked(bool s);

    QString checkBoxText() const;
    void setCheckBoxText(const QString &);

    bool isCheckBoxVisible() const;
    void setCheckBoxVisible(bool);

    QDialogButtonBox::StandardButtons standardButtons() const;
    void setStandardButtons(QDialogButtonBox::StandardButtons s);
    QPushButton *button(QDialogButtonBox::StandardButton b) const;
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role);

    QDialogButtonBox::StandardButton defaultButton() const;
    void setDefaultButton(QDialogButtonBox::StandardButton s);

    // See static QMessageBox::standardPixmap()
    QPixmap iconPixmap() const;
    void setIconPixmap (const QPixmap &p);

    // Query the result
    QAbstractButton *clickedButton() const;
    QDialogButtonBox::StandardButton clickedStandardButton() const;

    // Conversion convenience
    static QMessageBox::StandardButton dialogButtonBoxToMessageBoxButton(QDialogButtonBox::StandardButton);

private slots:
    void slotClicked(QAbstractButton *b);

private:
    CheckableMessageBoxPrivate *d;
};

#endif // CHECKABLEMESSAGEBOX_H
