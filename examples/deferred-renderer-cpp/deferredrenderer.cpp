/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "deferredrenderer.h"


DeferredRenderer::DeferredRenderer(Qt3D::QNode *parent)
    : Qt3D::QViewport(parent)
    , m_clearGBuffer(new Qt3D::QClearBuffer())
    , m_clearScreenQuad(new Qt3D::QClearBuffer())
    , m_sceneFilter(new Qt3D::QLayerFilter())
    , m_screenQuadFilter(new Qt3D::QLayerFilter())
    , m_gBufferTargetSelector(new Qt3D::QRenderTargetSelector())
    , m_geometryPassFilter(new Qt3D::QRenderPassFilter())
    , m_finalPassFilter(new Qt3D::QRenderPassFilter())
    , m_sceneCameraSelector(new Qt3D::QCameraSelector())
{
    m_geometryPassFilter->addChild(m_sceneCameraSelector);
    m_clearGBuffer->addChild(m_geometryPassFilter);
    m_gBufferTargetSelector->addChild(m_clearGBuffer);
    m_sceneFilter->addChild(m_gBufferTargetSelector);
    addChild(m_sceneFilter);

    m_clearScreenQuad->addChild(m_finalPassFilter);
    m_screenQuadFilter->addChild(m_clearScreenQuad);
    addChild(m_screenQuadFilter);

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

void DeferredRenderer::setGeometryPassCriteria(QList<Qt3D::QCriterion *> criteria)
{
    Q_FOREACH (Qt3D::QCriterion *c, criteria)
        m_geometryPassFilter->addCriterion(c);
}

void DeferredRenderer::setFinalPassCriteria(QList<Qt3D::QCriterion *> criteria)
{
    Q_FOREACH (Qt3D::QCriterion *c, criteria)
        m_finalPassFilter->addChild(c);
}

void DeferredRenderer::setGBufferLayer(const QString &layerName)
{
    m_sceneFilter->setLayers(QStringList(layerName));
}

void DeferredRenderer::setScreenQuadLayer(const QString &layerName)
{
    m_screenQuadFilter->setLayers(QStringList(layerName));
}
