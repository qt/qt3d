// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_SCENE3DRENDERER_P_H
#define QT3DRENDER_SCENE3DRENDERER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/qsize.h>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <scene3ditem_p.h>

QT_BEGIN_NAMESPACE

class QRhi;
class QRhiTextureRenderTarget;
class QRhiRenderBuffer;
class QRhiTexture;
class QRhiRenderPassDescriptor;
class QQuickWindow;
class QSGTexture;
class QOpenGLFramebufferObject;

namespace Qt3DCore {
class QAspectEngine;
}

namespace Qt3DRender {

class QRenderAspect;
class Scene3DCleaner;
class Scene3DSGNode;

namespace Render {
class AbstractRenderer;
}

class Scene3DRenderer : public QObject
{
    Q_OBJECT
public:
    Scene3DRenderer();
    ~Scene3DRenderer();

    Scene3DSGNode *sgNode() const;
    void setCleanerHelper(Scene3DCleaner *cleaner);
    void allowRender();
    void setCompositingMode(Scene3DItem::CompositingMode mode);
    void setSkipFrame(bool skip);
    void init(Scene3DItem *item, Qt3DCore::QAspectEngine *aspectEngine, QRenderAspect *renderAspect);

    void setMultisample(bool multisample);
    void setBoundingSize(const QSize &size);

    bool multisample() const { return m_multisample; }
    QSize boundingSize() const { return m_boundingRectSize; }
    bool isYUp() const;

    void init(Qt3DCore::QAspectEngine *aspectEngine, QRenderAspect *renderAspect);

    void beforeSynchronize();
    void setWindow(QQuickWindow *window);

    bool hasShutdown() const { return !m_needsShutdown; }

    QRenderAspect *renderAspect() const { return m_renderAspect; }
public Q_SLOTS:
    void shutdown();

private:

    class QuickRenderer
    {
    public:
        // ctor/dtor cannot be inlined because of QScopedPointer with forward
        // declared class
        QuickRenderer();
        virtual ~QuickRenderer();
        virtual void initialize(Scene3DRenderer *scene3DRenderer,
                                Qt3DRender::Render::AbstractRenderer *renderer) = 0;
        virtual void beforeSynchronize(Scene3DRenderer *scene3DRenderer) = 0;
        virtual void beforeRendering(Scene3DRenderer *scene3DRenderer) = 0;
        virtual void beforeRenderPassRecording(Scene3DRenderer *scene3DRenderer) = 0;
        virtual void shutdown(Scene3DRenderer *sceneRenderer) = 0;
        virtual bool isYUp() const = 0;

    protected:
        bool m_lastMultisample = false;
        QSize m_lastSize;
        QScopedPointer<QSGTexture> m_texture;
        Qt3DRender::Render::AbstractRenderer *m_renderer = nullptr;
    };

    class GLRenderer : public QuickRenderer
    {
    public:
        QOpenGLFramebufferObject *createMultisampledFramebufferObject(const QSize &size);
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);

        void initialize(Scene3DRenderer *scene3DRenderer,
                        Qt3DRender::Render::AbstractRenderer *renderer) override;
        void beforeSynchronize(Scene3DRenderer *scene3DRenderer) override;
        void beforeRendering(Scene3DRenderer *scene3DRenderer) override;
        void beforeRenderPassRecording(Scene3DRenderer *scene3DRenderer) override;
        void shutdown(Scene3DRenderer *sceneRenderer) override;
        bool isYUp() const override { return true; };

    private:
        QScopedPointer<QOpenGLFramebufferObject> m_multisampledFBO;
        QScopedPointer<QOpenGLFramebufferObject> m_finalFBO;
        bool m_multisample = false;
        quint32 m_textureId = 0;
    };

    class RHIRenderer : public QuickRenderer
    {
    public:
        void initialize(Scene3DRenderer *scene3DRenderer,
                        Qt3DRender::Render::AbstractRenderer *renderer) override;
        void beforeSynchronize(Scene3DRenderer *scene3DRenderer) override;
        void beforeRendering(Scene3DRenderer *scene3DRenderer) override;
        void beforeRenderPassRecording(Scene3DRenderer *scene3DRenderer) override;
        void shutdown(Scene3DRenderer *sceneRenderer) override;
        bool isYUp() const override;

    private:
        void releaseRHIResources();

        QRhiTexture *m_rhiTexture = nullptr;
        QRhiRenderBuffer *m_rhiColorRenderBuffer = nullptr;
        QRhiRenderBuffer *m_rhiMSAARenderBuffer = nullptr;
        QRhiRenderBuffer *m_rhiDepthRenderBuffer = nullptr;
        QRhiTextureRenderTarget *m_rhiRenderTarget = nullptr;
        QRhiRenderPassDescriptor *m_rhiRenderTargetPassDescriptor = nullptr;
        QRhi *m_rhi = nullptr;
    };

    Qt3DCore::QAspectEngine *m_aspectEngine; // Will be released by the Scene3DItem
    QRenderAspect *m_renderAspect; // Will be released by the aspectEngine
    Scene3DSGNode *m_node; // Will be released by the QtQuick SceneGraph
    QQuickWindow *m_window;
    QMutex m_windowMutex;

    QSize m_lastSize;
    QSize m_boundingRectSize;
    bool m_multisample;
    bool m_lastMultisample;

    bool m_needsShutdown;
    bool m_shouldRender;
    bool m_dirtyViews;
    bool m_skipFrame;
    QSemaphore m_allowRendering;
    Scene3DItem::CompositingMode m_compositingMode;
    bool m_resetRequested = false;
    QuickRenderer *m_quickRenderer = nullptr;

    friend class Scene3DItem;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_SCENE3DRENDERER_P_H
