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

#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DRenderer/qrenderattachment.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderTarget::RenderTarget()
    : QBackendNode()
{
}

void RenderTarget::updateFromPeer(QNode *peer)
{
    QRenderTarget *target = static_cast<QRenderTarget *>(peer);
    m_renderAttachments.clear();
    Q_FOREACH (QRenderAttachment *att, target->attachments())
        appendRenderAttachment(att->id());
}

void RenderTarget::cleanup()
{
}

void RenderTarget::appendRenderAttachment(const QNodeId &attachmentId)
{
    if (!m_renderAttachments.contains(attachmentId))
        m_renderAttachments.append(attachmentId);
}

void RenderTarget::removeRenderAttachment(const QNodeId &attachmentId)
{
    m_renderAttachments.removeOne(attachmentId);
}

QList<QNodeId> RenderTarget::renderAttachments() const
{
    return m_renderAttachments;
}

void RenderTarget::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeAdded && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        appendRenderAttachment(propertyChange->value().value<QNodeId>());
    else if (e->type() == NodeRemoved && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        removeRenderAttachment(propertyChange->value().value<QNodeId>());
}

} // Render

} // Qt3D

QT_END_NAMESPACE
