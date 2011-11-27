#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <qwidget.h>

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent = 0);

    enum CurrentWidget {
        Search,
        Result
    };

    virtual void setCurrentWidget(CurrentWidget index)=0;
    virtual void toggleWidget()=0;
    
};

#endif // SEARCHWIDGET_H
