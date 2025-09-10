// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "additiveclipblend_p.h"
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/private/qadditiveclipblend_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

AdditiveClipBlend::AdditiveClipBlend()
    : ClipBlendNode(ClipBlendNode::AdditiveBlendType)
    , m_baseClipId()
    , m_additiveClipId()
    , m_additiveFactor(0.0f)
{
}

AdditiveClipBlend::~AdditiveClipBlend()
{
}

void AdditiveClipBlend::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAdditiveClipBlend *node = qobject_cast<const QAdditiveClipBlend *>(frontEnd);
    if (!node)
        return;

    m_additiveFactor = node->additiveFactor();
    m_baseClipId = Qt3DCore::qIdForNode(node->baseClip());
    m_additiveClipId = Qt3DCore::qIdForNode(node->additiveClip());
}

ClipResults AdditiveClipBlend::doBlend(const QList<ClipResults> &blendData) const
{
    Q_ASSERT(blendData.size() == 2);
    Q_ASSERT(blendData[0].size() == blendData[1].size());
    const qsizetype elementCount = blendData.first().size();
    ClipResults blendResults(elementCount);

    for (qsizetype i = 0; i < elementCount; ++i)
        blendResults[i] = blendData[0][i] + m_additiveFactor * blendData[1][i];

    return blendResults;
}

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE
