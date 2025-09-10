// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK3DSCENE2D_SCENE2D_P_H
#define QT3DRENDER_RENDER_QUICK3DSCENE2D_SCENE2D_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DQuickScene2D/qscene2d.h>

#include <private/qscene2d_p.h>
#include <private/qrendertargetoutput_p.h>
#include <private/backendnode_p.h>
#include <private/attachmentpack_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class GraphicsContext;

namespace Quick {

class Scene2D;

class RenderQmlEventHandler : public QObject
{
    Q_OBJECT
public:
    RenderQmlEventHandler(Scene2D *node);
    bool event(QEvent *e) override;

private:
    Scene2D *m_node;
};

class Q_3DQUICKSCENE2DSHARED_EXPORT Scene2D : public Qt3DRender::Render::BackendNode
{
public:
    Scene2D();
    ~Scene2D();

    void render();
    void initializeRender();
    void setSharedObject(Qt3DRender::Quick::Scene2DSharedObjectPtr sharedObject);
    void cleanup();
    void setOutput(Qt3DCore::QNodeId outputId);
    void initializeSharedObject();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    bool updateFbo(QOpenGLTexture *texture);
    void syncRenderControl();
    bool registerObjectPickerEvents(Qt3DCore::QEntity *qentity);
    void unregisterObjectPickerEvents(Qt3DCore::QNodeId entityId);
    void handlePickEvent(int type, const QPickEvent *ev);

    QOpenGLContext *m_context;
    QOpenGLContext *m_shareContext;
    QThread *m_renderThread;
    Qt3DCore::QNodeId m_outputId;
    QSharedPointer<Qt3DRender::Quick::Scene2DSharedObject> m_sharedObject;
    Qt3DCore::QNodeId m_peerId;
    Qt3DRender::Render::Attachment m_attachmentData;

    GLuint m_fbo;
    GLuint m_rbo;
    QSize m_textureSize;

    bool m_initialized;
    bool m_renderInitialized;
    bool m_mouseEnabled;
    Qt3DRender::Quick::QScene2D::RenderPolicy m_renderPolicy;
    QList<Qt3DCore::QNodeId> m_entities;
    Qt3DRender::QPickEventPtr m_cachedPickEvent;
    QList<QMetaObject::Connection> m_connections;
};

} // Quick
} // Render
} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK3DSCENE2D_SCENE2D_P_H
