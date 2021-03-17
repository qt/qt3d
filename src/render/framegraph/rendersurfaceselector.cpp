/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
