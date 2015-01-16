/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qforwardrenderer.h"
#include "qforwardrenderer_p.h"

#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qclearbuffer.h>
#include <Qt3DRenderer/qannotation.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QForwardRendererPrivate::QForwardRendererPrivate(QForwardRenderer *qq)
    : QTechniqueFilterPrivate(qq)
    , m_viewport(new QViewport())
    , m_cameraSelector(new QCameraSelector())
    , m_clearBuffer(new QClearBuffer())
{
}

void QForwardRendererPrivate::init()
{
    Q_Q(QForwardRenderer);

    m_clearBuffer->setParent(m_cameraSelector);
    m_cameraSelector->setParent(m_viewport);
    m_viewport->setParent(q);

    m_viewport->setRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    m_viewport->setClearColor(Qt::white);
    m_clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);

    QAnnotation *forwardRenderingStyle = new QAnnotation(q);
    forwardRenderingStyle->setName(QStringLiteral("renderingStyle"));
    forwardRenderingStyle->setValue(QStringLiteral("forward"));
    q->addRequirement(forwardRenderingStyle);
}

/*!
    \class Qt3D::QForwardRenderer
    \brief The Qt3D::QForwardRenderer provides a default FrameGraph implementation of a forward renderer.
    \inmodule Qt3DRender
    \since 5.5

    Forward rendering is how OpenGL is traditionally. It renders directly to the backbuffer
    one object at a time shading each one as it goes.

    Internally the Qt3D::QForwardRenderer is a subclass of Qt3D::QTechniqueFilter.
    This a is a single leaf Framegraph tree which contains a Qt3D::QViewport, a Qt3D::QCameraSelector
    and a Qt3D::QClearBuffer.
    The Qt3D::QForwardRenderer has a default requirement annotation whose name is "renderingStyle" and value "forward".
    If you need to filter out your techniques, you should do so based on that annocation.

    By default the viewport occupies the whole screen and the clear color is white.
*/

/*!
    \fn Qt3D::QForwardRenderer::QForwardRenderer(QNode *parent)
    Constructs a new Qt3D::QForwardRenderer instance with parent object \a parent.
 */
QForwardRenderer::QForwardRenderer(QNode *parent)
    : QTechniqueFilter(*new QForwardRendererPrivate(this), parent)
{
    Q_D(QForwardRenderer);
    QObject::connect(d->m_viewport, SIGNAL(clearColorChanged()), this, SIGNAL(clearColorChanged()));
    QObject::connect(d->m_viewport, SIGNAL(rectChanged()), this, SIGNAL(viewportRectChanged()));
    QObject::connect(d->m_cameraSelector, SIGNAL(cameraChanged()), this, SIGNAL(cameraChanged()));
    d->init();
}

QForwardRenderer::~QForwardRenderer()
{
}

/*!
    \fn Qt3D::QForwardRenderer::setViewportRect(const QRectF &viewportRect)
    Sets the current viewport rect to \a viewportRect.
 */
void QForwardRenderer::setViewportRect(const QRectF &viewportRect)
{
    Q_D(QForwardRenderer);
    d->m_viewport->setRect(viewportRect);
}

/*!
    \fn Qt3D::QForwardRenderer::setClearColor(const QColor &clearColor)
    Sets the clearColor to \a clearColor.
 */
void QForwardRenderer::setClearColor(const QColor &clearColor)
{
    Q_D(QForwardRenderer);
    d->m_viewport->setClearColor(clearColor);
}

/*!
    \fn Qt3D::QForwardRenderer::setCamera(QEntity *camera)
    Sets the camera which should be used to render the scene to \a camera.
    \b {Note:} A camera is a QEntity having a QCameraLens has one of its components
 */
void QForwardRenderer::setCamera(QEntity *camera)
{
    Q_D(QForwardRenderer);
    d->m_cameraSelector->setCamera(camera);
}

/*!
    \fn Qt3D::QForwardRenderer::viewportRect() const
    \return the current viewport rect.
 */
QRectF QForwardRenderer::viewportRect() const
{
    Q_D(const QForwardRenderer);
    return d->m_viewport->rect();
}

/*!
    \fn Qt3D::QForwardRenderer::clearColor() const
    \return the current clearColor.
 */
QColor QForwardRenderer::clearColor() const
{
    Q_D(const QForwardRenderer);
    return d->m_viewport->clearColor();
}

/*!
    \fn Qt3D::QForwardRenderer::camera() const
    \return the current QEntity camera used to render the scene.
 */
QEntity *QForwardRenderer::camera() const
{
    Q_D(const QForwardRenderer);
    return d->m_cameraSelector->camera();
}

} // Qt3D

QT_END_NAMESPACE
