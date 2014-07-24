/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quick3dframegraphitem.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DFrameGraphItem::Quick3DFrameGraphItem(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QFrameGraphItem> Quick3DFrameGraphItem::frameGraphChildrenList()
{
    return QQmlListProperty<QFrameGraphItem>(this, 0,
                                             &Quick3DFrameGraphItem::apprendFrameGraphItem,
                                             &Quick3DFrameGraphItem::frameGraphItemsCount,
                                             &Quick3DFrameGraphItem::frameGraphItemAt,
                                             &Quick3DFrameGraphItem::clearFrameGraphItemList);
}

void Quick3DFrameGraphItem::apprendFrameGraphItem(QQmlListProperty<QFrameGraphItem> *list, QFrameGraphItem *item)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->appendFrameGraphItem(item);
}

QFrameGraphItem *Quick3DFrameGraphItem::frameGraphItemAt(QQmlListProperty<QFrameGraphItem> *list, int index)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->frameGraphChildren().at(index);
    return Q_NULLPTR;
}

int Quick3DFrameGraphItem::frameGraphItemsCount(QQmlListProperty<QFrameGraphItem> *list)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->frameGraphChildren().count();
    return -1;
}

void Quick3DFrameGraphItem::clearFrameGraphItemList(QQmlListProperty<QFrameGraphItem> *list)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg) {
        Q_FOREACH (QFrameGraphItem *item, fg->parentFrameGraphItem()->frameGraphChildren())
            fg->parentFrameGraphItem()->removeFrameGraphItem(item);
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

