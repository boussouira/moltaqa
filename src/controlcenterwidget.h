#ifndef ABSTRACTEDITWIDGET_H
#define ABSTRACTEDITWIDGET_H

#include <qwidget.h>
#include "libraryinfo.h"

class LibraryManager;

class ControlCenterWidget : public QWidget
{
    Q_OBJECT

public:
    ControlCenterWidget(QWidget* parent = 0);

    virtual QString title()=0;

    virtual void loadModel()=0;
    virtual void save()=0;

    virtual void aboutToShow();
    virtual void aboutToHide();

    void setModified(bool m);
    bool isModified();

signals:
    void modified(bool e);

protected:
    LibraryManager *m_libraryManager;
    bool m_modified;
};

#endif // ABSTRACTEDITWIDGET_H
