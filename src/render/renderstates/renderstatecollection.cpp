/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include "Qt3DRender/private/renderstatecollection_p.h"
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/managers_p.h>

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

RenderStateCollection::RenderStateCollection()
    : m_dirty(false)
{
}

RenderStateCollection::~RenderStateCollection()
{
}

void RenderStateCollection::setDirty(bool dirty)
{
    QMutexLocker lock(&m_mutex);
    m_dirty = dirty;
}

QVector<RenderStateNode*> RenderStateCollection::renderStates(RenderStateManager *manager) const
{
    QMutexLocker lock(&m_mutex);
    if (m_dirty) {
        m_renderStateNodes.clear();

        for (const Qt3DCore::QNodeId id : m_renderStateIds) {
            RenderStateNode *node = manager->lookupResource(id);
            if (node)
               m_renderStateNodes.append(node);
            else
                m_renderStateIds.removeAll(id);
        }

        m_dirty = false;
    }

    return m_renderStateNodes;
}

bool RenderStateCollection::hasRenderStates() const
{
    QMutexLocker lock(&m_mutex);
    return !m_renderStateIds.empty();
}

void RenderStateCollection::appendRenderState(Qt3DCore::QNodeId renderStateId)
{
    QMutexLocker lock(&m_mutex);
    if (!m_renderStateIds.contains(renderStateId)) {
        m_renderStateIds.append(renderStateId);
        m_dirty = true;
    }
}

void RenderStateCollection::removeRenderState(Qt3DCore::QNodeId renderStateId)
{
    QMutexLocker lock(&m_mutex);
    if (m_renderStateIds.removeAll(renderStateId) > 0) {
        m_dirty = true;
    }

}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
