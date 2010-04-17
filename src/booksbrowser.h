#ifndef BOOKSBROWSER_H
#define BOOKSBROWSER_H

#include <QDialog>

namespace Ui {
    class BooksBrowser;
}

class BooksBrowser : public QDialog {
    Q_OBJECT
public:
    BooksBrowser(QWidget *parent = 0);
    ~BooksBrowser();

private:
    Ui::BooksBrowser *ui;
};

#endif // BOOKSBROWSER_H
