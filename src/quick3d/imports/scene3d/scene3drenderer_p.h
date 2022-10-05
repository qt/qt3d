/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
class Scene3DViews;

class Scene3DRenderer : public QObject
{
    Q_OBJECT
public:
    Scene3DRenderer();
    ~Scene3DRenderer();

    void setSGNode(Scene3DSGNode *node);
    void setCleanerHelper(Scene3DCleaner *cleaner);
    void allowRender();
    void setCompositingMode(Scene3DItem::CompositingMode mode);
    void setSkipFrame(bool skip);
    void setMultisample(bool multisample);
    void setBoundingSize(const QSize &size);

    void setScene3DViews(const QVector<Scene3DView *> views);
    void init(Qt3DCore::QAspectEngine *aspectEngine, QRenderAspect *renderAspect);

    void beforeSynchronize();
    void setWindow(QQuickWindow *window);

    bool hasShutdown() const { return !m_needsShutdown; }

    QRenderAspect *renderAspect() const { return m_renderAspect; }
public Q_SLOTS:
    void render();
    void shutdown();

private:
    QOpenGLFramebufferObject *createMultisampledFramebufferObject(const QSize &size);
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);

    Qt3DCore::QAspectEngine *m_aspectEngine; // Will be released by the Scene3DItem
    QRenderAspect *m_renderAspect; // Will be released by the aspectEngine
    QScopedPointer<QOpenGLFramebufferObject> m_multisampledFBO;
    QScopedPointer<QOpenGLFramebufferObject> m_finalFBO;
    QScopedPointer<QSGTexture> m_texture;
    Scene3DSGNode *m_node; // Will be released by the QtQuick SceneGraph
    QQuickWindow *m_window;
    QMutex m_windowMutex;
    QSize m_lastSize;
    QSize m_boundingRectSize;
    bool m_multisample;
    bool m_lastMultisample;
    bool m_needsShutdown;
    bool m_forceRecreate;
    bool m_shouldRender;
    bool m_dirtyViews;
    bool m_skipFrame;
    bool m_skippedLastFrame;
    QSemaphore m_allowRendering;
    Scene3DItem::CompositingMode m_compositingMode;
    QVector<Scene3DView *> m_views;
    bool m_resetRequested = false;

    friend class Scene3DItem;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_SCENE3DRENDERER_P_H
