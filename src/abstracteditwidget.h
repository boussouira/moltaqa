#ifndef ABSTRACTEDITWIDGET_H
#define ABSTRACTEDITWIDGET_H

#include <qwidget.h>

class AbstractEditWidget : public QWidget
{
    Q_OBJECT

public:
    AbstractEditWidget(QWidget* parent = 0);

public slots:
    virtual void save()=0;

signals:
    void edited(bool e);
};

#endif // ABSTRACTEDITWIDGET_H
