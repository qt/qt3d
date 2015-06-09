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

#include "qviewport.h"
#include "qviewport_p.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QViewportPrivate
    \internal
*/
QViewportPrivate::QViewportPrivate()
    : QFrameGraphNodePrivate()
    , m_rect(QRectF(0.0f, 0.0f, 1.0f, 1.0f))
{
}

void QViewport::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QViewport *viewport = static_cast<const QViewport*>(ref);
    d_func()->m_rect = viewport->d_func()->m_rect;
}

QViewport::QViewport(QNode *parent)
    : QFrameGraphNode(*new QViewportPrivate, parent)
{
}

QViewport::~QViewport()
{
    QNode::cleanup();
}

/*! \internal */
QViewport::QViewport(QViewportPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QRectF QViewport::rect() const
{
    Q_D(const QViewport);
    return d->m_rect;
}

void QViewport::setRect(const QRectF &rect)
{
    Q_D(QViewport);
    if (rect != d->m_rect) {
        d->m_rect = rect;
        emit rectChanged();
    }
}

QColor QViewport::clearColor() const
{
    Q_D(const QViewport);
    return d->m_clearColor;
}

void QViewport::setClearColor(const QColor &color)
{
    Q_D(QViewport);
    if (color != d->m_clearColor) {
        d->m_clearColor = color;
        emit clearColorChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
