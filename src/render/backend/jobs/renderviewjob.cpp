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

#include "renderviewjob_p.h"

#include <Qt3DRenderer/private/renderview_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderviewjobutils_p.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

void RenderViewJob::run()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_index;

    // Create a RenderView object
    // The RenderView are created from a QFrameAllocator stored in the current Thread local storage

    QFrameAllocator *currentFrameAllocator = m_renderer->currentFrameAllocator();
    RenderView *renderView = currentFrameAllocator->allocate<RenderView>();

    // RenderView should allocate heap resources using only the currentFrameAllocator
    renderView->setAllocator(currentFrameAllocator);
    renderView->setRenderer(m_renderer);
    renderView->setSurfaceSize(m_surfaceSize);

    // Populate the renderview's configuration from the framegraph
    setRenderViewConfigFromFrameGraphLeafNode(renderView, m_fgLeaf);

    // Build RenderCommand should perform the culling as we have no way to determine
    // if a child has a mesh in the view frustum while its parent isn't contained in it.
    if (!renderView->noDraw())
        renderView->buildRenderCommands(m_renderer->renderSceneRoot());

    // Sorts RenderCommand
    renderView->sort();

    // Enqueue our fully populated RenderView with the RenderThread
    m_renderer->enqueueRenderView(renderView, m_index);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
