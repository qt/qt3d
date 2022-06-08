// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "rendersurfaceselector_p.h"
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/private/qrendersurfaceselector_p.h>
#include <QtGui/qwindow.h>
#include <QtGui/qscreen.h>
#include <QtGui/qoffscreensurface.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace {

QSurface *surfaceFromQObject(QObject *o)
{
    QSurface *surface = nullptr;
    QWindow *window = qobject_cast<QWindow *>(o);
    if (window) {
        surface = static_cast<QSurface *>(window);
    } else {
        QOffscreenSurface *offscreen = qobject_cast<QOffscreenSurface *>(o);
        if (offscreen)
            surface = static_cast<QSurface *>(offscreen);
    }
    return surface;
}

}

namespace Qt3DRender {
namespace Render {

RenderSurfaceSelector::RenderSurfaceSelector()
    : FrameGraphNode(FrameGraphNode::Surface)
    , m_surfaceObj(nullptr)
    , m_surface(nullptr)
    , m_width(0)
    , m_height(0)
    , m_devicePixelRatio(0.0f)
{
}

void RenderSurfaceSelector::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QRenderSurfaceSelector *node = qobject_cast<const QRenderSurfaceSelector *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->surface() != m_surfaceObj) {
        m_surfaceObj = node->surface();
        m_surface = surfaceFromQObject(m_surfaceObj);
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    if (m_surface && m_surface->surfaceClass() == QSurface::Window) {
        QWindow *window = static_cast<QWindow *>(m_surface);
        if (window->width() != m_width) {
            m_width = window->width();
            markDirty(AbstractRenderer::FrameGraphDirty);
        }
        if (window->height() != m_height) {
            m_height = window->height();
            markDirty(AbstractRenderer::FrameGraphDirty);
        }
    }

    if (node->externalRenderTargetSize() != m_renderTargetSize) {
        m_renderTargetSize = node->externalRenderTargetSize();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    if (node->surfacePixelRatio() != m_devicePixelRatio) {
        m_devicePixelRatio = node->surfacePixelRatio();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

QSize RenderSurfaceSelector::renderTargetSize() const
{
    if (m_renderTargetSize.isValid())
        return m_renderTargetSize;
    return QSize(m_width, m_height);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
