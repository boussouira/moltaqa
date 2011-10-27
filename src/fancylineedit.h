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

#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

#include <QtCore/qglobal.h>

#include <QtGui/QLineEdit>

class FancyLineEditPrivate;

/* A line edit with an embedded pixmap on one side that is connected to
 * a menu. Additionally, it can display a grayed hintText (like "Type Here to")
 * when not focused and empty. When connecting to the changed signals and
 * querying text, one has to be aware that the text is set to that hint
 * text if isShowingHintText() returns true (that is, does not contain
 * valid user input).
 */
class FancyLineEdit : public QLineEdit
{
    Q_DISABLE_COPY(FancyLineEdit)
    Q_OBJECT
    Q_ENUMS(Side)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap DESIGNABLE true)
    Q_PROPERTY(Side side READ side WRITE setSide DESIGNABLE isSideStored STORED isSideStored)
    Q_PROPERTY(bool useLayoutDirection READ useLayoutDirection WRITE setUseLayoutDirection DESIGNABLE true)
    Q_PROPERTY(bool menuTabFocusTrigger READ hasMenuTabFocusTrigger WRITE setMenuTabFocusTrigger  DESIGNABLE true)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText DESIGNABLE true)
    Q_PROPERTY(bool extraPadding READ extraPadding WRITE setExtraPadding DESIGNABLE true)

public:
    enum Side {Left, Right};

    explicit FancyLineEdit(QWidget *parent = 0);
    ~FancyLineEdit();

    QPixmap pixmap() const;

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setSide(Side side);
    Side side() const;

    bool useLayoutDirection() const;
    void setUseLayoutDirection(bool v);

    // Set whether tabbing in will trigger the menu.
    bool hasMenuTabFocusTrigger() const;
    void setMenuTabFocusTrigger(bool v);

    void setPlaceholderText(const QString &text);
    QString placeholderText();
    
    void setExtraPadding(bool extra);
    bool extraPadding();

signals:
    void buttonClicked();

public slots:
    void setPixmap(const QPixmap &pixmap);

protected:
    virtual void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void changeEvent(QEvent *e);

private:
    friend class FancyLineEditPrivate;
    bool isSideStored() const;
    void updateMenuLabel();
    void positionMenuLabel();
    void updateStyleSheet(Side side);

    FancyLineEditPrivate *m_d;
    QString m_placeText;
    bool m_extraPadding;
};

#endif // FANCYLINEEDIT_H
