/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
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

QQmlListProperty<QFrameGraphNode> Quick3DFrameGraphItem::frameGraphChildrenList()
{
    return QQmlListProperty<QFrameGraphNode>(this, 0,
                                             &Quick3DFrameGraphItem::apprendFrameGraphItem,
                                             &Quick3DFrameGraphItem::frameGraphItemsCount,
                                             &Quick3DFrameGraphItem::frameGraphItemAt,
                                             &Quick3DFrameGraphItem::clearFrameGraphItemList);
}

void Quick3DFrameGraphItem::apprendFrameGraphItem(QQmlListProperty<QFrameGraphNode> *list, QFrameGraphNode *item)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->appendFrameGraphNode(item);
}

QFrameGraphNode *Quick3DFrameGraphItem::frameGraphItemAt(QQmlListProperty<QFrameGraphNode> *list, int index)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->frameGraphChildren().at(index);
    return Q_NULLPTR;
}

int Quick3DFrameGraphItem::frameGraphItemsCount(QQmlListProperty<QFrameGraphNode> *list)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg)
        return fg->parentFrameGraphItem()->frameGraphChildren().count();
    return -1;
}

void Quick3DFrameGraphItem::clearFrameGraphItemList(QQmlListProperty<QFrameGraphNode> *list)
{
    Quick3DFrameGraphItem *fg = qobject_cast<Quick3DFrameGraphItem *>(list->object);
    if (fg) {
        Q_FOREACH (QFrameGraphNode *item, fg->parentFrameGraphItem()->frameGraphChildren())
            fg->parentFrameGraphItem()->removeFrameGraphNode(item);
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

