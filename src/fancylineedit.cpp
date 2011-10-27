/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "fancylineedit.h"

#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QLabel>

enum { margin = 6 };

static inline QString sideToStyleSheetString(FancyLineEdit::Side side)
{
    return side == FancyLineEdit::Left ? QLatin1String("left") : QLatin1String("right");
}

// Format style sheet for the label containing the pixmap. It has a margin on
// the outer side of the whole FancyLineEdit.
static QString labelStyleSheet(FancyLineEdit::Side side)
{
    QString rc = QLatin1String("QLabel { margin-");
    rc += sideToStyleSheetString(side);
    rc += QLatin1String(": ");
    rc += QString::number(margin);
    rc += QLatin1Char('}');
    return rc;
}

// --------- FancyLineEditPrivate as QObject with label
//           event filter

class FancyLineEditPrivate : public QObject {
public:
    explicit FancyLineEditPrivate(FancyLineEdit *parent);

    virtual bool eventFilter(QObject *obj, QEvent *event);

    const QString m_leftLabelStyleSheet;
    const QString m_rightLabelStyleSheet;

    FancyLineEdit  *m_lineEdit;
    QPixmap m_pixmap;
    QMenu *m_menu;
    QLabel *m_menuLabel;
    FancyLineEdit::Side m_side;
    bool m_useLayoutDirection;
    bool m_menuTabFocusTrigger;
};


FancyLineEditPrivate::FancyLineEditPrivate(FancyLineEdit *parent) :
    QObject(parent),
    m_leftLabelStyleSheet(labelStyleSheet(FancyLineEdit::Left)),
    m_rightLabelStyleSheet(labelStyleSheet(FancyLineEdit::Right)),
    m_lineEdit(parent),
    m_menu(0),
    m_menuLabel(0),
    m_side(FancyLineEdit::Left),
    m_useLayoutDirection(true),
    m_menuTabFocusTrigger(false)
{
}

bool FancyLineEditPrivate::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != m_menuLabel)
        return QObject::eventFilter(obj, event);

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
            const QMouseEvent *me = static_cast<QMouseEvent *>(event);
            if (m_menu) {
                m_menu->exec(me->globalPos());
            } else {
                emit m_lineEdit->buttonClicked();
            }
            return true;
        }
    case QEvent::FocusIn:
        if (m_menuTabFocusTrigger && m_menu) {
            m_lineEdit->setFocus();
            m_menu->exec(m_menuLabel->mapToGlobal(m_menuLabel->rect().center()));
            return true;
        }
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

// --------- FancyLineEdit
FancyLineEdit::FancyLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_d(new FancyLineEditPrivate(this)),
    m_extraPadding(false)
{
    m_d->m_menuLabel = new QLabel(this);
    m_d->m_menuLabel->installEventFilter(m_d);
    updateMenuLabel();
}

FancyLineEdit::~FancyLineEdit()
{
}

// Position the menu label left or right according to size.
// Called when switching side and from resizeEvent.
void FancyLineEdit::positionMenuLabel()
{
    if ((side() == Left && layoutDirection() == Qt::LeftToRight)||
        (side() == Right && layoutDirection() == Qt::RightToLeft))
        m_d->m_menuLabel->setGeometry(0, 0, m_d->m_pixmap.width()+margin, height());
    else
        m_d->m_menuLabel->setGeometry(width() - m_d->m_pixmap.width() - margin, 0,
                                      m_d->m_pixmap.width()+margin, height());
}

void FancyLineEdit::updateStyleSheet(Side side)
{
    // Udate the LineEdit style sheet. Make room for the label on the
    // respective side and set color according to whether we are showing the
    // hint text
    QString sheet = QLatin1String("QLineEdit{ padding-");
    sheet += sideToStyleSheetString(side);
    sheet += QLatin1String(": ");
    sheet += QString::number(m_d->m_pixmap.width() + margin);
    sheet += QLatin1String("px");
    sheet += QLatin1Char(';');
    
    if(m_extraPadding)
        sheet += QLatin1String("padding-bottom:2px;padding-top:3px;");
    
    sheet += QLatin1Char('}');
    setStyleSheet(sheet);

    setMinimumSize(m_d->m_pixmap.width() + margin, m_d->m_pixmap.height() + margin);
}

void FancyLineEdit::updateMenuLabel()
{
    m_d->m_menuLabel->setPixmap(m_d->m_pixmap);
    const Side s = side();
    switch (s) {
    case Left:
        m_d->m_menuLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        m_d->m_menuLabel->setStyleSheet(m_d->m_leftLabelStyleSheet);
        break;
    case Right:
        m_d->m_menuLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        m_d->m_menuLabel->setStyleSheet(m_d->m_rightLabelStyleSheet);
        break;
    }
    updateStyleSheet(s);
    positionMenuLabel();

    m_d->m_menuLabel->setCursor(QCursor(Qt::PointingHandCursor));

    if(m_extraPadding)
        setMinimumSize(sizeHint());
}

void FancyLineEdit::setSide(Side side)
{
    m_d->m_side = side;
    updateMenuLabel();
}

FancyLineEdit::Side FancyLineEdit::side() const
{
    if (m_d->m_useLayoutDirection)
        return qApp->layoutDirection() == Qt::LeftToRight ? Left : Right;
    return  m_d->m_side;
}

void FancyLineEdit::resizeEvent(QResizeEvent *)
{
    positionMenuLabel();
}

void FancyLineEdit::focusInEvent(QFocusEvent *e)
{
    if(e->reason() == Qt::MouseFocusReason) {
        if(text() == m_placeText && styleSheet() == "color:gray;") {
            setStyleSheet("color:black;");
            setText("");
        }
    }

    QLineEdit::focusInEvent(e);
}

void FancyLineEdit::focusOutEvent(QFocusEvent *e)
{
    if(e->reason() == Qt::MouseFocusReason) {
        if(text().isEmpty()) {
            setStyleSheet("color:gray;");
            setText(m_placeText);
        }
    }

    QLineEdit::focusOutEvent(e);
}

void FancyLineEdit::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LayoutDirectionChange)
        positionMenuLabel();

    QLineEdit::changeEvent(e);
}

void FancyLineEdit::setPixmap(const QPixmap &pixmap)
{
    m_d->m_pixmap = pixmap;
    updateMenuLabel();
}

QPixmap FancyLineEdit::pixmap() const
{
    return m_d->m_pixmap;
}

void FancyLineEdit::setMenu(QMenu *menu)
{
     m_d->m_menu = menu;
}

QMenu *FancyLineEdit::menu() const
{
    return  m_d->m_menu;
}
void FancyLineEdit::setPlaceholderText(const QString &text)
{
    m_placeText = text;
}

QString FancyLineEdit::placeholderText()
{
    return m_placeText;
}

bool FancyLineEdit::useLayoutDirection() const
{
    return m_d->m_useLayoutDirection;
}

void FancyLineEdit::setExtraPadding(bool extra)
{
    m_extraPadding = extra;
    updateMenuLabel();
}

bool FancyLineEdit::extraPadding()
{
    return m_extraPadding;
}

void FancyLineEdit::setUseLayoutDirection(bool v)
{
    m_d->m_useLayoutDirection = v;
}

bool FancyLineEdit::isSideStored() const
{
    return !m_d->m_useLayoutDirection;
}

bool FancyLineEdit::hasMenuTabFocusTrigger() const
{
    return m_d->m_menuTabFocusTrigger;
}

void FancyLineEdit::setMenuTabFocusTrigger(bool v)
{
    if (m_d->m_menuTabFocusTrigger == v)
        return;

    m_d->m_menuTabFocusTrigger = v;
    m_d->m_menuLabel->setFocusPolicy(v ? Qt::TabFocus : Qt::NoFocus);
}
