/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "deferredrenderer.h"


DeferredRenderer::DeferredRenderer(Qt3DCore::QNode *parent)
    : Qt3DRender::QViewport(parent)
    , m_surfaceSelector(new Qt3DRender::QRenderSurfaceSelector(this))
    , m_sceneFilter(new Qt3DRender::QLayerFilter(m_surfaceSelector))
    , m_screenQuadFilter(new Qt3DRender::QLayerFilter(m_surfaceSelector))
    , m_clearScreenQuad(new Qt3DRender::QClearBuffers(m_screenQuadFilter))
    , m_gBufferTargetSelector(new Qt3DRender::QRenderTargetSelector(m_sceneFilter))
    , m_clearGBuffer(new Qt3DRender::QClearBuffers(m_gBufferTargetSelector))
    , m_geometryPassFilter(new Qt3DRender::QRenderPassFilter(m_clearGBuffer))
    , m_finalPassFilter(new Qt3DRender::QRenderPassFilter(m_clearScreenQuad))
    , m_sceneCameraSelector(new Qt3DRender::QCameraSelector(m_geometryPassFilter))
{
    m_clearGBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    m_clearScreenQuad->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
}

void DeferredRenderer::setSceneCamera(Qt3DCore::QEntity *camera)
{
    m_sceneCameraSelector->setCamera(camera);
}

void DeferredRenderer::setGBuffer(Qt3DRender::QRenderTarget *gBuffer)
{
    m_gBufferTargetSelector->setTarget(gBuffer);
}

void DeferredRenderer::setGeometryPassCriteria(QList<Qt3DRender::QFilterKey *> criteria)
{
    Q_FOREACH (Qt3DRender::QFilterKey *c, criteria)
        m_geometryPassFilter->addMatch(c);
}

void DeferredRenderer::setFinalPassCriteria(QList<Qt3DRender::QFilterKey *> criteria)
{
    Q_FOREACH (Qt3DRender::QFilterKey *c, criteria)
        c->setParent(m_finalPassFilter);
}

void DeferredRenderer::setGBufferLayer(Qt3DRender::QLayer *layer)
{
    m_sceneFilter->addLayer(layer);
}

void DeferredRenderer::setScreenQuadLayer(Qt3DRender::QLayer *layer)
{
    m_screenQuadFilter->addLayer(layer);
}
