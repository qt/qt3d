/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "floatingitem_sg.h"
#include "floatingitemnode_sg.h"
#include <QtDeclarative/qsgcontext.h>
#include <QtDeclarative/private/qsgitem_p.h>
#include <QApplication>
#include <QDesktopWidget>

QT_BEGIN_NAMESPACE

class FloatingItemSGPrivate : public QSGItemPrivate
{
    Q_DECLARE_PUBLIC(FloatingItemSG)
public:
    FloatingItemSGPrivate()
        : depth(0.0f)
    {
    }

    TransformNode *createTransformNode();

    qreal depth;
};

FloatingItemSG::FloatingItemSG(QSGItem *parent)
    : QSGItem(*new FloatingItemSGPrivate, parent)
{
    setFlag(ItemHasContents);
}

FloatingItemSG::~FloatingItemSG()
{
}

qreal FloatingItemSG::depth() const
{
    Q_D(const FloatingItemSG);
    return d->depth;
}

void FloatingItemSG::setDepth(qreal depth)
{
    Q_D(FloatingItemSG);
    if (d->depth != depth) {
        d->depth = depth;
        emit depthChanged();
        update();
    }
}

QRectF FloatingItemSG::boundingRect() const
{
    Q_D(const FloatingItemSG);
    QRectF bounds = QSGItem::boundingRect();
    qreal depth = qAbs(d->depth);
    depth = depth * qApp->desktop()->logicalDpiX() / 100.0f;
    return bounds.adjusted(-depth, -depth, depth, depth);
}

Node *FloatingItemSG::updatePaintNode(Node *node, UpdatePaintNodeData *data)
{
    Q_D(FloatingItemSG);
    FloatingItemSGNode *fNode = static_cast<FloatingItemSGNode *>(data->transformNode);
    fNode->setDepth(d->depth);
    return QSGItem::updatePaintNode(node, data);
}

TransformNode *FloatingItemSGPrivate::createTransformNode()
{
    return new FloatingItemSGNode(QSGContext::current);
}

QT_END_NAMESPACE
