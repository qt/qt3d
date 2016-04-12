/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qclearbuffers.h"
#include "qclearbuffers_p.h"
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QClearBuffersPrivate::QClearBuffersPrivate()
    : QFrameGraphNodePrivate()
    , m_buffersType(QClearBuffers::None)
    , m_clearDepthValue(1.f)
    , m_clearStencilValue(0)
{
}

void QClearBuffers::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QClearBuffers *b = static_cast<const QClearBuffers*>(ref);
    d_func()->m_buffersType = b->d_func()->m_buffersType;
    d_func()->m_clearColor = b->d_func()->m_clearColor;
    d_func()->m_clearDepthValue = b->d_func()->m_clearDepthValue;
    d_func()->m_clearStencilValue = b->d_func()->m_clearStencilValue;
}

QClearBuffers::QClearBuffers(QNode *parent)
    : QFrameGraphNode(*new QClearBuffersPrivate, parent)
{
}

/*! \internal */
QClearBuffers::QClearBuffers(QClearBuffersPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}


QClearBuffers::BufferType QClearBuffers::buffers() const
{
    Q_D(const QClearBuffers);
    return d->m_buffersType;
}

QColor QClearBuffers::clearColor() const
{
    Q_D(const QClearBuffers);
    return d->m_clearColor;
}

float QClearBuffers::clearDepthValue() const
{
    Q_D(const QClearBuffers);
    return d->m_clearDepthValue;
}

int QClearBuffers::clearStencilValue() const
{
    Q_D(const QClearBuffers);
    return d->m_clearStencilValue;
}

void QClearBuffers::setBuffers(QClearBuffers::BufferType buffers)
{
    Q_D(QClearBuffers);
    if (d->m_buffersType != buffers) {
        d->m_buffersType = buffers;
        emit buffersChanged(buffers);
    }
}

void QClearBuffers::setClearColor(const QColor &color)
{
    Q_D(QClearBuffers);
    if (d->m_clearColor != color) {
        d->m_clearColor = color;
        emit clearColorChanged(color);
    }
}

void QClearBuffers::setClearDepthValue(float clearDepthValue)
{
    Q_D(QClearBuffers);
    if (d->m_clearDepthValue != clearDepthValue) {
        if (clearDepthValue >= 0.f && clearDepthValue <= 1.f) {
            d->m_clearDepthValue = clearDepthValue;
            emit clearDepthValueChanged(clearDepthValue);
        } else qWarning() << "Invalid clear depth value";
    }
}

void QClearBuffers::setClearStencilValue(int clearStencilValue)
{
    Q_D(QClearBuffers);
    if (d->m_clearStencilValue != clearStencilValue) {
        d->m_clearStencilValue = clearStencilValue;
        emit clearStencilValueChanged(clearStencilValue);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QClearBuffers::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QClearBuffersData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QClearBuffers);
    data.buffersType = d->m_buffersType;
    data.clearColor = d->m_clearColor;
    data.clearDepthValue = d->m_clearDepthValue;
    data.clearStencilValue = d->m_clearStencilValue;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
