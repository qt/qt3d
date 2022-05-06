/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qspritesheetitem.h"
#include "qspritesheetitem_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DExtras {

QSpriteSheetItemPrivate::QSpriteSheetItemPrivate()
    : QNodePrivate()
    , m_x(0)
    , m_y(0)
    , m_width(0)
    , m_height(0)
{

}

QSpriteSheetItem::QSpriteSheetItem(QNode *parent)
    : Qt3DCore::QNode(* new QSpriteSheetItemPrivate(), parent)
{

}

int QSpriteSheetItem::x() const
{
    Q_D(const QSpriteSheetItem);
    return d->m_x;
}

void QSpriteSheetItem::setX(int x)
{
    Q_D(QSpriteSheetItem);
    if (x != d->m_x) {
        d->m_x = x;
        emit xChanged(x);
    }
}

int QSpriteSheetItem::y() const
{
    Q_D(const QSpriteSheetItem);
    return d->m_y;
}

void QSpriteSheetItem::setY(int y)
{
    Q_D(QSpriteSheetItem);
    if (y != d->m_y) {
        d->m_y = y;
        emit yChanged(y);
    }
}

int QSpriteSheetItem::width() const
{
    Q_D(const QSpriteSheetItem);
    return d->m_width;
}

void QSpriteSheetItem::setWidth(int width)
{
    Q_D(QSpriteSheetItem);
    if (width != d->m_width) {
        d->m_width = width;
        emit widthChanged(width);
    }
}

int QSpriteSheetItem::height() const
{
    Q_D(const QSpriteSheetItem);
    return d->m_height;
}

void QSpriteSheetItem::setHeight(int height)
{
    Q_D(QSpriteSheetItem);
    if (height != d->m_height) {
        d->m_height = height;
        emit heightChanged(height);
    }
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
