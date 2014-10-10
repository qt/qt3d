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

#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DRenderer/qrenderattachment.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/rendereraspect.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderTarget::RenderTarget()
    : m_renderer(Q_NULLPTR)
{
}

void RenderTarget::setPeer(QRenderTarget *peer)
{
    QUuid peerUuid;

    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (peerUuid != m_renderTargetUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_renderTargetUuid.isNull()) {
            arbiter->unregisterObserver(this, m_renderTargetUuid);
            m_renderAttachments.clear();
        }
        m_renderTargetUuid = peerUuid;
        if (!m_renderTargetUuid.isNull()) {
            arbiter->registerObserver(this, m_renderTargetUuid, NodeAdded|NodeRemoved|NodeUpdated);
            Q_FOREACH (QRenderAttachment *att, peer->attachments())
                appendRenderAttachment(att);
        }
    }
}

void RenderTarget::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderTarget::cleanup()
{
    if (m_renderer != Q_NULLPTR && !m_renderTargetUuid.isNull())
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_renderTargetUuid);
}

void RenderTarget::appendRenderAttachment(QRenderAttachment *attachment)
{
    if (!m_renderAttachments.contains(attachment->uuid()))
        m_renderAttachments.append(attachment->uuid());
}

void RenderTarget::removeRenderAttachment(const QUuid &attachmentId)
{
    m_renderAttachments.removeOne(attachmentId);
}

QList<QUuid> RenderTarget::renderAttachments() const
{
    return m_renderAttachments;
}

QUuid RenderTarget::renderTargetUuid() const
{
    return m_renderTargetUuid;
}

void RenderTarget::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeAdded && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        appendRenderAttachment(propertyChange->value().value<QRenderAttachment *>());
    else if (e->type() == NodeRemoved && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        removeRenderAttachment(propertyChange->value().value<QRenderAttachment *>()->uuid());
}

} // Render

} // Qt3D

QT_END_NAMESPACE
