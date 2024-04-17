// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "deferredrenderer.h"
#include "gbuffer.h"
#include <Qt3DRender/QParameter>
#include <QWindow>

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
    , m_winSize(new Qt3DRender::QParameter(QStringLiteral("winSize"), QSizeF(1024.0f, 768.0f)))
    , m_gBuffer(new GBuffer(this))
    , m_window(nullptr)
{
    m_clearGBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    m_clearScreenQuad->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    m_gBufferTargetSelector->setTarget(m_gBuffer);

    m_finalPassFilter->addParameter(new Qt3DRender::QParameter(QStringLiteral("position"), m_gBuffer->positionTexture()));
    m_finalPassFilter->addParameter(new Qt3DRender::QParameter(QStringLiteral("normal"), m_gBuffer->normalTexture()));
    m_finalPassFilter->addParameter(new Qt3DRender::QParameter(QStringLiteral("color"), m_gBuffer->colorTexture()));
    m_finalPassFilter->addParameter(m_winSize);
}

void DeferredRenderer::setSceneCamera(Qt3DCore::QEntity *camera)
{
    m_sceneCameraSelector->setCamera(camera);
}

void DeferredRenderer::setGeometryPassCriteria(QList<Qt3DRender::QFilterKey *> criteria)
{
    for (Qt3DRender::QFilterKey *c : std::as_const(criteria))
        m_geometryPassFilter->addMatch(c);
}

void DeferredRenderer::setFinalPassCriteria(QList<Qt3DRender::QFilterKey *> criteria)
{
    for (Qt3DRender::QFilterKey *c : std::as_const(criteria))
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

void DeferredRenderer::setSurface(QWindow *surface)
{
    if (surface != m_window) {

        // Disconnect old window's signals
        if (m_window != nullptr) {
            QObject::disconnect(m_widthChangedConnection);
            QObject::disconnect(m_heightChangedConnection);
        }

        m_window = surface;
        m_surfaceSelector->setSurface(surface);

        if (m_window != nullptr) {
            // Store connections
            m_widthChangedConnection = QObject::connect(surface, &QWindow::widthChanged,
                                                        this, [this](int width) {
                                                            m_winSize->setValue(QSizeF(float(width), m_winSize->value().toSizeF().height()));
                                                        });
            m_heightChangedConnection = QObject::connect(surface, &QWindow::heightChanged,
                                                         this, [this](int height) {
                                                             m_winSize->setValue(QSizeF(m_winSize->value().toSizeF().width(), float(height)));
                                                         });
        }
    }
}
