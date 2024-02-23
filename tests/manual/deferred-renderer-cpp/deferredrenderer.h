// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <Qt3DRender/QViewport>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QFilterKey>

class GBuffer;

class DeferredRenderer : public Qt3DRender::QViewport
{
public:
    explicit DeferredRenderer(Qt3DCore::QNode *parent = 0);

    void setSceneCamera(Qt3DCore::QEntity *camera);
    void setGBuffer(Qt3DRender::QRenderTarget *gBuffer);
    void setGeometryPassCriteria(QList<Qt3DRender::QFilterKey *> criteria);
    void setFinalPassCriteria(QList<Qt3DRender::QFilterKey *> criteria);
    void setGBufferLayer(Qt3DRender::QLayer *layer);
    void setScreenQuadLayer(Qt3DRender::QLayer *layer);
    void setSurface(QWindow *surface);

private:
    Qt3DRender::QRenderSurfaceSelector *m_surfaceSelector;
    Qt3DRender::QLayerFilter *m_sceneFilter;
    Qt3DRender::QLayerFilter *m_screenQuadFilter;
    Qt3DRender::QClearBuffers *m_clearScreenQuad;
    Qt3DRender::QRenderTargetSelector *m_gBufferTargetSelector;
    Qt3DRender::QClearBuffers *m_clearGBuffer;
    Qt3DRender::QRenderPassFilter *m_geometryPassFilter;
    Qt3DRender::QRenderPassFilter *m_finalPassFilter;
    Qt3DRender::QCameraSelector *m_sceneCameraSelector;
    Qt3DRender::QParameter *m_winSize;
    GBuffer *m_gBuffer;
    QWindow *m_window;

    QMetaObject::Connection m_widthChangedConnection;
    QMetaObject::Connection m_heightChangedConnection;
};

#endif // DEFERREDRENDERER_H
