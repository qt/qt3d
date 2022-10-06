// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
                               const QList<QRenderTargetOutput::AttachmentPoint> &drawBuffers)
{
    // Copy attachments
    const auto outputIds = target->renderOutputs();
    for (Qt3DCore::QNodeId outputId : outputIds) {
        const RenderTargetOutput *output = attachmentManager->lookupResource(outputId);
        if (output)
            m_attachments.push_back(*output->attachment());
    }

    // Sort by attachment point
    std::sort(m_attachments.begin(),
              m_attachments.end(),
              [] (const Attachment &a, const Attachment &b) {
            return a.m_point < b.m_point;
    });


    // Create actual DrawBuffers list that is used for glDrawBuffers

    // If nothing is specified, use all the attachments as draw buffers
    if (drawBuffers.empty()) {
        m_drawBuffers.reserve(m_attachments.size());
        for (const Attachment &attachment : std::as_const(m_attachments))
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
    for (size_t i = 0; i < m_drawBuffers.size(); i++)
        if (m_drawBuffers.at(i) == (int)attachmentPoint)
            return int(i);
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
