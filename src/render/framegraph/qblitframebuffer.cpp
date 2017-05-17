/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qblitframebuffer.h"
#include "qblitframebuffer_p.h"

#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/qframegraphnodecreatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QBlitFramebufferPrivate::QBlitFramebufferPrivate()
    : QFrameGraphNodePrivate()
    , m_source(nullptr)
    , m_destination(nullptr)
    , m_sourceRect(QRect())
    , m_destinationRect(QRect())
    , m_sourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0)
    , m_destinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0)
    , m_interpolationMethod(QBlitFramebuffer::Linear)
{
}

QBlitFramebuffer::QBlitFramebuffer(QNode *parent)
    : QFrameGraphNode(*new QBlitFramebufferPrivate, parent)
{

}

QBlitFramebuffer::QBlitFramebuffer(QBlitFramebufferPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QBlitFramebuffer::~QBlitFramebuffer()
{

}

QRenderTarget *QBlitFramebuffer::source() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_source;
}

QRenderTarget *QBlitFramebuffer::destination() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_destination;
}

QRectF QBlitFramebuffer::sourceRect() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_sourceRect;
}

QRectF QBlitFramebuffer::destinationRect() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_destinationRect;
}

Qt3DRender::QRenderTargetOutput::AttachmentPoint QBlitFramebuffer::sourceAttachmentPoint() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_sourceAttachmentPoint;
}

QRenderTargetOutput::AttachmentPoint QBlitFramebuffer::destinationAttachmentPoint() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_destinationAttachmentPoint;
}

QBlitFramebuffer::InterpolationMethod QBlitFramebuffer::interpolationMethod() const
{
    Q_D(const QBlitFramebuffer);
    return d->m_interpolationMethod;
}

void QBlitFramebuffer::setSource(QRenderTarget *source)
{
    Q_D(QBlitFramebuffer);
    if (d->m_source != source) {
        d->m_source = source;
        emit sourceChanged();
    }
}

void QBlitFramebuffer::setDestination(QRenderTarget *destination)
{
    Q_D(QBlitFramebuffer);
    if (d->m_destination != destination) {
        d->m_destination = destination;
        emit destinationChanged();
    }
}

void QBlitFramebuffer::setSourceRect(const QRectF &inputRect)
{
    Q_D(QBlitFramebuffer);
    if (d->m_sourceRect != inputRect) {
        d->m_sourceRect = inputRect.toRect();
        emit sourceRectChanged();
    }
}

void QBlitFramebuffer::setDestinationRect(const QRectF &outputRect)
{
    Q_D(QBlitFramebuffer);
    if (d->m_destinationRect != outputRect) {
        d->m_destinationRect = outputRect.toRect();
        emit destinationRectChanged();
    }
}

void QBlitFramebuffer::setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint)
{
    Q_D(QBlitFramebuffer);
    if (d->m_sourceAttachmentPoint != sourceAttachmentPoint) {
        d->m_sourceAttachmentPoint = sourceAttachmentPoint;
        emit sourceAttachmentPointChanged();
    }
}

void QBlitFramebuffer::setDestinationAttachmentPoint(QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint)
{
    Q_D(QBlitFramebuffer);
    if (d->m_destinationAttachmentPoint != destinationAttachmentPoint) {
        d->m_destinationAttachmentPoint = destinationAttachmentPoint;
        emit destinationAttachmentPointChanged();
    }
}

void QBlitFramebuffer::setInterpolationMethod(QBlitFramebuffer::InterpolationMethod interpolationMethod)
{
    Q_D(QBlitFramebuffer);
    if (d->m_interpolationMethod != interpolationMethod) {
        d->m_interpolationMethod = interpolationMethod;
        emit interpolationMethodChanged();
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QBlitFramebuffer::createNodeCreationChange() const
{
    auto creationChange = QFrameGraphNodeCreatedChangePtr<QBlitFramebufferData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QBlitFramebuffer);
    data.m_sourceRect = d->m_sourceRect;
    data.m_destinationRect = d->m_destinationRect;
    data.m_sourceRenderTargetId = Qt3DCore::qIdForNode(d->m_source);
    data.m_destinationRenderTargetId = Qt3DCore::qIdForNode(d->m_destination);
    data.m_sourceAttachmentPoint = d->m_sourceAttachmentPoint;
    data.m_destinationAttachmentPoint = d->m_destinationAttachmentPoint;
    data.m_interpolationMethod = d->m_interpolationMethod;

    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
