/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qforwardrenderer.h"
#include "qforwardrenderer_p.h"

#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qrendersurfaceselector.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace Qt3DExtras {

QForwardRendererPrivate::QForwardRendererPrivate()
    : QTechniqueFilterPrivate()
    , m_surfaceSelector(new QRenderSurfaceSelector)
    , m_viewport(new QViewport())
    , m_cameraSelector(new QCameraSelector())
    , m_clearBuffer(new QClearBuffers())
    , m_frustumCulling(new QFrustumCulling())
{
}

void QForwardRendererPrivate::init()
{
    Q_Q(QForwardRenderer);

    m_frustumCulling->setParent(m_clearBuffer);
    m_clearBuffer->setParent(m_cameraSelector);
    m_cameraSelector->setParent(m_viewport);
    m_viewport->setParent(m_surfaceSelector);
    m_surfaceSelector->setParent(q);

    m_viewport->setNormalizedRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    m_clearBuffer->setClearColor(Qt::white);
    m_clearBuffer->setBuffers(QClearBuffers::ColorDepthBuffer);

    QFilterKey *forwardRenderingStyle = new QFilterKey(q);
    forwardRenderingStyle->setName(QStringLiteral("renderingStyle"));
    forwardRenderingStyle->setValue(QStringLiteral("forward"));
    q->addMatch(forwardRenderingStyle);
}

/*!
    \class Qt3DRender::QForwardRenderer
    \brief The Qt3DRender::QForwardRenderer provides a default \l {QFrameGraph}{FrameGraph} implementation of a forward renderer.
    \inmodule Qt3DRender
    \since 5.5

    Forward rendering is how OpenGL is traditionally. It renders directly to the backbuffer
    one object at a time shading each one as it goes.

    Internally the Qt3DRender::QForwardRenderer is a subclass of Qt3DRender::QTechniqueFilter.
    This a is a single leaf Framegraph tree which contains a Qt3DRender::QViewport, a Qt3DRender::QCameraSelector
    and a Qt3DRender::QClearBuffers.
    The Qt3DRender::QForwardRenderer has a default requirement annotation whose name is "renderingStyle" and value "forward".
    If you need to filter out your techniques, you should do so based on that annotation.

    By default the viewport occupies the whole screen and the clear color is white. Frustum culling is also enabled.
*/

/*!
    Constructs a new Qt3DRender::QForwardRenderer instance with parent object \a parent.
 */
QForwardRenderer::QForwardRenderer(QNode *parent)
    : QTechniqueFilter(*new QForwardRendererPrivate, parent)
{
    Q_D(QForwardRenderer);
    QObject::connect(d->m_clearBuffer, SIGNAL(clearColorChanged(const QColor &)), this, SIGNAL(clearColorChanged(const QColor &)));
    QObject::connect(d->m_viewport, SIGNAL(normalizedRectChanged(const QRectF &)), this, SIGNAL(viewportRectChanged(const QRectF &)));
    QObject::connect(d->m_cameraSelector, SIGNAL(cameraChanged(Qt3DCore::QEntity *)), this, SIGNAL(cameraChanged(Qt3DCore::QEntity *)));
    QObject::connect(d->m_surfaceSelector, SIGNAL(surfaceChanged(QObject *)), this, SIGNAL(surfaceChanged(QObject *)));
    d->init();
}

/*!
    Destroys the QForwardRenderer instance.
*/
QForwardRenderer::~QForwardRenderer()
{
}

void QForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
    Q_D(QForwardRenderer);
    d->m_viewport->setNormalizedRect(viewportRect);
}

void QForwardRenderer::setClearColor(const QColor &clearColor)
{
    Q_D(QForwardRenderer);
    d->m_clearBuffer->setClearColor(clearColor);
}

/*!
    Sets the camera which should be used to render the scene to \a camera.

    \note A camera is a QEntity having a QCameraLens as one of its components.
*/
void QForwardRenderer::setCamera(Qt3DCore::QEntity *camera)
{
    Q_D(QForwardRenderer);
    d->m_cameraSelector->setCamera(camera);
}

void QForwardRenderer::setSurface(QObject *surface)
{
    Q_D(QForwardRenderer);
    d->m_surfaceSelector->setSurface(surface);
}

/*!
    \property Qt3DRender::QForwardRenderer::viewportRect

    Holds the current viewport normalizedRect.
 */
QRectF QForwardRenderer::viewportRect() const
{
    Q_D(const QForwardRenderer);
    return d->m_viewport->normalizedRect();
}

/*!
    \property Qt3DRender::QForwardRenderer::clearColor

    Holds the current clearColor.
*/
QColor QForwardRenderer::clearColor() const
{
    Q_D(const QForwardRenderer);
    return d->m_clearBuffer->clearColor();
}

/*!
    \property Qt3DRender::QForwardRenderer::camera

    Holds the current QEntity camera used to render the scene.

    \note A camera is a QEntity that has a QCameraLens as one of its components.
*/
Qt3DCore::QEntity *QForwardRenderer::camera() const
{
    Q_D(const QForwardRenderer);
    return d->m_cameraSelector->camera();
}

QObject *QForwardRenderer::surface() const
{
    Q_D(const QForwardRenderer);
    return d->m_surfaceSelector->surface();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
