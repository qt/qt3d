// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "clipblendvalue_p.h"
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/private/qclipblendvalue_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ClipBlendValue::ClipBlendValue()
    : ClipBlendNode(ValueType)
{
}

ClipBlendValue::~ClipBlendValue()
{
}

void ClipBlendValue::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QClipBlendValue *node = qobject_cast<const QClipBlendValue *>(frontEnd);
    if (!node)
        return;

    m_clipId = Qt3DCore::qIdForNode(node->clip());
}

ClipResults ClipBlendValue::doBlend(const QList<ClipResults> &blendData) const
{
    // Should never be called for the value node
    Q_UNUSED(blendData);
    Q_UNREACHABLE_RETURN(ClipResults());
}

double ClipBlendValue::duration() const
{
    if (m_clipId.isNull())
        return 0.0;
    AnimationClip *clip = m_handler->animationClipLoaderManager()->lookupResource(m_clipId);
    Q_ASSERT(clip);
    return clip->duration();
}

void ClipBlendValue::setClipFormat(Qt3DCore::QNodeId animatorId, const ClipFormat &formatIndices)
{
    // Do we already have an entry for this animator?
    const int animatorIndex = m_animatorIds.indexOf(animatorId);
    if (animatorIndex == -1) {
        // Nope, add it
        m_animatorIds.push_back(animatorId);
        m_clipFormats.push_back(formatIndices);
    } else {
        m_clipFormats[animatorIndex] = formatIndices;
    }
}

ClipFormat &ClipBlendValue::clipFormat(Qt3DCore::QNodeId animatorId)
{
    const int animatorIndex = m_animatorIds.indexOf(animatorId);
    return m_clipFormats[animatorIndex];
}

const ClipFormat &ClipBlendValue::clipFormat(Qt3DCore::QNodeId animatorId) const
{
    const int animatorIndex = m_animatorIds.indexOf(animatorId);
    return m_clipFormats[animatorIndex];
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
