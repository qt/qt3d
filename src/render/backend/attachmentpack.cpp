/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include "attachmentpack_p.h"
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

AttachmentPack::AttachmentPack()
{
}

AttachmentPack::AttachmentPack(const RenderTarget *target,
                               AttachmentManager *attachmentManager,
                               const QVector<QRenderTargetOutput::AttachmentPoint> &drawBuffers)
{
    // Copy attachments
    const auto outputIds = target->renderOutputs();
    for (Qt3DCore::QNodeId outputId : outputIds) {
        const RenderTargetOutput *output = attachmentManager->lookupResource(outputId);
        if (output)
            m_attachments.append(*output->attachment());
    }

    // Create actual DrawBuffers list that is used for glDrawBuffers

    // If nothing is specified, use all the attachments as draw buffers
    if (drawBuffers.isEmpty()) {
        m_drawBuffers.reserve(m_attachments.size());
        for (const Attachment &attachment : qAsConst(m_attachments))
            // only consider Color Attachments
            if (attachment.m_point <= QRenderTargetOutput::Color15)
                m_drawBuffers.push_back((int) attachment.m_point);
    } else {
        m_drawBuffers.reserve(drawBuffers.size());
        for (QRenderTargetOutput::AttachmentPoint drawBuffer : drawBuffers)
            if (drawBuffer <= QRenderTargetOutput::Color15)
                m_drawBuffers.push_back((int) drawBuffer);
    }
}

// return index of given attachment within actual draw buffers list
int AttachmentPack::getDrawBufferIndex(QRenderTargetOutput::AttachmentPoint attachmentPoint) const
{
    for (int i = 0; i < m_drawBuffers.size(); i++)
        if (m_drawBuffers.at(i) == (int)attachmentPoint)
            return i;
    return -1;
}

bool operator ==(const Attachment &a, const Attachment &b)
{
    return (a.m_name == b.m_name &&
            a.m_mipLevel == b.m_mipLevel &&
            a.m_layer == b.m_layer &&
            a.m_textureUuid == b.m_textureUuid &&
            a.m_point == b.m_point &&
            a.m_face == b.m_face);
}

bool operator !=(const Attachment &a, const Attachment &b)
{
    return !(a == b);
}

bool operator ==(const AttachmentPack &packA, const AttachmentPack &packB)
{
    return (packA.attachments() == packB.attachments() &&
            packA.getGlDrawBuffers() == packB.getGlDrawBuffers());
}

bool operator !=(const AttachmentPack &packA, const AttachmentPack &packB)
{
    return !(packA == packB);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
