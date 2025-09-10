// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "sortpolicy_p.h"
#include <Qt3DRender/private/qsortpolicy_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

SortPolicy::SortPolicy()
    : FrameGraphNode(FrameGraphNode::SortMethod)
{
}

void SortPolicy::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QSortPolicy *node = qobject_cast<const QSortPolicy *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const auto sortTypes = node->sortTypes();
    if (sortTypes != m_sortTypes) {
        m_sortTypes = sortTypes;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

QList<QSortPolicy::SortType> SortPolicy::sortTypes() const
{
    return m_sortTypes;
}

} // namepace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
