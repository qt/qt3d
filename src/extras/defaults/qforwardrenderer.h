// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QFORWARDRENDERER_H
#define QT3DEXTRAS_QFORWARDRENDERER_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DCore/qentity.h>
#include <QtCore/QRectF>
#include <QtGui/QColor>
#include <Qt3DRender/QClearBuffers>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DExtras {

class QForwardRendererPrivate;

class Q_3DEXTRASSHARED_EXPORT QForwardRenderer : public Qt3DRender::QTechniqueFilter
{
    Q_OBJECT
    Q_PROPERTY(QObject *surface READ surface WRITE setSurface NOTIFY surfaceChanged)
    Q_PROPERTY(QObject *window READ surface WRITE setSurface NOTIFY surfaceChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(Qt3DRender::QClearBuffers::BufferType buffersToClear READ buffersToClear WRITE setBuffersToClear NOTIFY buffersToClearChanged REVISION 14)
    Q_PROPERTY(Qt3DCore::QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QSize externalRenderTargetSize READ externalRenderTargetSize WRITE setExternalRenderTargetSize NOTIFY externalRenderTargetSizeChanged)
    Q_PROPERTY(bool frustumCulling READ isFrustumCullingEnabled WRITE setFrustumCullingEnabled NOTIFY frustumCullingEnabledChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged REVISION 9)
    Q_PROPERTY(bool showDebugOverlay READ showDebugOverlay WRITE setShowDebugOverlay NOTIFY showDebugOverlayChanged REVISION 15)
public:
    explicit QForwardRenderer(Qt3DCore::QNode *parent = nullptr);
    ~QForwardRenderer();

    QRectF viewportRect() const;
    QColor clearColor() const;
    Qt3DRender::QClearBuffers::BufferType buffersToClear() const;
    Qt3DCore::QEntity *camera() const;
    QObject *surface() const;
    QSize externalRenderTargetSize() const;
    bool isFrustumCullingEnabled() const;
    float gamma() const;
    bool showDebugOverlay() const;

public Q_SLOTS:
    void setViewportRect(const QRectF &viewportRect);
    void setClearColor(const QColor &clearColor);
    void setBuffersToClear(Qt3DRender::QClearBuffers::BufferType);
    void setCamera(Qt3DCore::QEntity *camera);
    void setSurface(QObject * surface);
    void setExternalRenderTargetSize(const QSize &size);
    void setFrustumCullingEnabled(bool enabled);
    void setGamma(float gamma);
    void setShowDebugOverlay(bool showDebugOverlay);

Q_SIGNALS:
    void viewportRectChanged(const QRectF &viewportRect);
    void clearColorChanged(const QColor &clearColor);
    void buffersToClearChanged(Qt3DRender::QClearBuffers::BufferType);
    void cameraChanged(Qt3DCore::QEntity *camera);
    void surfaceChanged(QObject *surface);
    void externalRenderTargetSizeChanged(const QSize &size);
    void frustumCullingEnabledChanged(bool enabled);
    void gammaChanged(float gamma);
    void showDebugOverlayChanged(bool showDebugOverlay);

private:
    Q_DECLARE_PRIVATE(QForwardRenderer)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QFORWARDRENDERER_H
