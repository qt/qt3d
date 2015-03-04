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

#include "deferredrenderer.h"


DeferredRenderer::DeferredRenderer(Qt3D::QNode *parent)
    : Qt3D::QViewport(parent)
    , m_sceneFilter(new Qt3D::QLayerFilter(this))
    , m_screenQuadFilter(new Qt3D::QLayerFilter(this))
    , m_clearScreenQuad(new Qt3D::QClearBuffer(m_screenQuadFilter))
    , m_gBufferTargetSelector(new Qt3D::QRenderTargetSelector(m_sceneFilter))
    , m_clearGBuffer(new Qt3D::QClearBuffer(m_gBufferTargetSelector))
    , m_geometryPassFilter(new Qt3D::QRenderPassFilter(m_clearGBuffer))
    , m_finalPassFilter(new Qt3D::QRenderPassFilter(m_clearScreenQuad))
    , m_sceneCameraSelector(new Qt3D::QCameraSelector(m_geometryPassFilter))
{
    m_clearGBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    m_clearScreenQuad->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
}

void DeferredRenderer::setSceneCamera(Qt3D::QEntity *camera)
{
    m_sceneCameraSelector->setCamera(camera);
}

void DeferredRenderer::setGBuffer(Qt3D::QRenderTarget *gBuffer)
{
    m_gBufferTargetSelector->setTarget(gBuffer);
}

void DeferredRenderer::setGeometryPassCriteria(QList<Qt3D::QAnnotation *> criteria)
{
    Q_FOREACH (Qt3D::QAnnotation *c, criteria)
        m_geometryPassFilter->addInclude(c);
}

void DeferredRenderer::setFinalPassCriteria(QList<Qt3D::QAnnotation *> criteria)
{
    Q_FOREACH (Qt3D::QAnnotation *c, criteria)
        c->setParent(m_finalPassFilter);
}

void DeferredRenderer::setGBufferLayers(const QStringList &layerNames)
{
    m_sceneFilter->setLayers(layerNames);
}

void DeferredRenderer::setScreenQuadLayers(const QStringList &layerNames)
{
    m_screenQuadFilter->setLayers(layerNames);
}
