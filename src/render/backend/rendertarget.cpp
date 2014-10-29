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
        appendRenderAttachment(att->uuid());
}

void RenderTarget::cleanup()
{
}

void RenderTarget::appendRenderAttachment(const QNodeUuid &attachmentId)
{
    if (!m_renderAttachments.contains(attachmentId))
        m_renderAttachments.append(attachmentId);
}

void RenderTarget::removeRenderAttachment(const QNodeUuid &attachmentId)
{
    m_renderAttachments.removeOne(attachmentId);
}

QList<QNodeUuid> RenderTarget::renderAttachments() const
{
    return m_renderAttachments;
}

void RenderTarget::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeAdded && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        appendRenderAttachment(propertyChange->value().value<QNodeUuid>());
    else if (e->type() == NodeRemoved && propertyChange->propertyName() == QByteArrayLiteral("attachment"))
        removeRenderAttachment(propertyChange->value().value<QNodeUuid>());
}

} // Render

} // Qt3D

QT_END_NAMESPACE
