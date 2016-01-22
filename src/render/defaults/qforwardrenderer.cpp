/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qforwardrenderer.h"
#include "qforwardrenderer_p.h"

#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qclearbuffer.h>
#include <Qt3DRender/qannotation.h>
#include <Qt3DRender/qfrustumculling.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QForwardRendererPrivate::QForwardRendererPrivate()
    : QTechniqueFilterPrivate()
    , m_viewport(new QViewport())
    , m_cameraSelector(new QCameraSelector())
    , m_clearBuffer(new QClearBuffer())
    , m_frustumCulling(new QFrustumCulling())
{
}

void QForwardRendererPrivate::init()
{
    Q_Q(QForwardRenderer);

    m_frustumCulling->setParent(m_clearBuffer);
    m_clearBuffer->setParent(m_cameraSelector);
    m_cameraSelector->setParent(m_viewport);
    m_viewport->setParent(q);

    m_viewport->setRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    m_viewport->setClearColor(Qt::white);
    m_clearBuffer->setBuffers(QClearBuffer::ColorDepthBuffer);

    QAnnotation *forwardRenderingStyle = new QAnnotation(q);
    forwardRenderingStyle->setName(QStringLiteral("renderingStyle"));
    forwardRenderingStyle->setValue(QStringLiteral("forward"));
    q->addRequirement(forwardRenderingStyle);
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
    and a Qt3DRender::QClearBuffer.
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
    QObject::connect(d->m_viewport, SIGNAL(clearColorChanged(const QColor &)), this, SIGNAL(clearColorChanged(const QColor &)));
    QObject::connect(d->m_viewport, SIGNAL(rectChanged(const QRectF &)), this, SIGNAL(viewportRectChanged(const QRectF &)));
    QObject::connect(d->m_cameraSelector, SIGNAL(cameraChanged(Qt3DCore::QEntity *)), this, SIGNAL(cameraChanged(Qt3DCore::QEntity *)));
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
    d->m_viewport->setRect(viewportRect);
}

void QForwardRenderer::setClearColor(const QColor &clearColor)
{
    Q_D(QForwardRenderer);
    d->m_viewport->setClearColor(clearColor);
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

/*!
    \property Qt3DRender::QForwardRenderer::viewportRect

    Holds the current viewport rect.
 */
QRectF QForwardRenderer::viewportRect() const
{
    Q_D(const QForwardRenderer);
    return d->m_viewport->rect();
}

/*!
    \property Qt3DRender::QForwardRenderer::clearColor

    Holds the current clearColor.
*/
QColor QForwardRenderer::clearColor() const
{
    Q_D(const QForwardRenderer);
    return d->m_viewport->clearColor();
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

} // namespace Qt3DRender

QT_END_NAMESPACE
