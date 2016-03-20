/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderviewjob_p.h"

#include <Qt3DRender/private/renderview_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/job_common_p.h>

#include <QElapsedTimer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {
// only accessed in ctor and dtor of RenderViewJob
// which are always being called in a non concurrent manner
int renderViewInstanceCounter = 0;
} // anonymous

RenderViewJob::RenderViewJob()
    : m_renderer(0)
    , m_devicePixelRatio(1.)
    , m_fgLeaf(0)
    , m_index(0)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderView, renderViewInstanceCounter++);
}

RenderViewJob::~RenderViewJob()
{
    renderViewInstanceCounter--;
}

void RenderViewJob::run()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_index;
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    QElapsedTimer timer;
    timer.start();
    qint64 gatherLightsTime;
    qint64 buildCommandsTime;
#endif

    // Create a RenderView object
    // The RenderView are created from a QFrameAllocator stored in the current Thread local storage

    Qt3DCore::QFrameAllocator *currentFrameAllocator = m_renderer->currentFrameAllocator();
    RenderView *renderView = currentFrameAllocator->allocate<RenderView>();

    // RenderView should allocate heap resources using only the currentFrameAllocator
    renderView->setAllocator(currentFrameAllocator);
    renderView->setRenderer(m_renderer);
    renderView->setSurfaceSize(m_surfaceSize);
    renderView->setDevicePixelRatio(m_devicePixelRatio);

    // Populate the renderview's configuration from the framegraph
    setRenderViewConfigFromFrameGraphLeafNode(renderView, m_fgLeaf);
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qint64 gatherStateTime = timer.nsecsElapsed();
    timer.restart();
#endif

    // Build RenderCommand should perform the culling as we have no way to determine
    // if a child has a mesh in the view frustum while its parent isn't contained in it.
    if (!renderView->noDraw()) {
        const QMatrix4x4 viewProjectionMatrix = renderView->viewProjectionMatrix();

        const Plane planes[6] = {
            Plane(viewProjectionMatrix.row(3) + viewProjectionMatrix.row(0)), // Left
            Plane(viewProjectionMatrix.row(3) - viewProjectionMatrix.row(0)), // Right
            Plane(viewProjectionMatrix.row(3) + viewProjectionMatrix.row(1)), // Top
            Plane(viewProjectionMatrix.row(3) - viewProjectionMatrix.row(1)), // Bottom
            Plane(viewProjectionMatrix.row(3) + viewProjectionMatrix.row(2)), // Front
            Plane(viewProjectionMatrix.row(3) - viewProjectionMatrix.row(2)), // Back
        };

        renderView->gatherLights(m_renderer->sceneRoot());
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
        gatherLightsTime = timer.nsecsElapsed();
        timer.restart();
#endif

        renderView->buildRenderCommands(m_renderer->sceneRoot(), planes);
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
        buildCommandsTime = timer.nsecsElapsed();
        timer.restart();
#endif
    }

#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qint64 creationTime = timer.nsecsElapsed();
    timer.restart();
#endif

    // Sorts RenderCommand
    renderView->sort();
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qint64 sortTime = timer.nsecsElapsed();
#endif

#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qDebug() << m_index
             << "state:" << gatherStateTime / 1.0e6
             << "lights:" << gatherLightsTime / 1.0e6
             << "build commands:" << buildCommandsTime / 1.0e6
             << "sort:" << sortTime / 1.0e6;
#endif

    // Enqueue our fully populated RenderView with the RenderThread
    m_renderer->enqueueRenderView(renderView, m_index);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
