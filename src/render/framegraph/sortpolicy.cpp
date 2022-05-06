/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#include "sortpolicy_p.h"
#include <Qt3DRender/private/qsortpolicy_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

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
