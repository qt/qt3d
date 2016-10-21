/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#ifndef QT3DRENDER_RENDER_SCENE2D_P_H
#define QT3DRENDER_RENDER_SCENE2D_P_H

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

#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/qscene2d_p.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QSemaphore>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class GraphicsContext;
class Scene2D;

class RenderQmlEventHandler : public QObject
{
    Q_OBJECT
public:
    RenderQmlEventHandler(Scene2D *node);
    bool event(QEvent *e) Q_DECL_OVERRIDE;

private:
    Scene2D *m_node;
};

class Q_AUTOTEST_EXPORT Scene2D : public FrameGraphNode
{
public:
    Scene2D();
    ~Scene2D();

    void attach();
    void render();
    void initializeRender();
    void setSharedObject(Scene2DSharedObjectPtr sharedObject);
    void cleanup();
    void setTexture(Qt3DCore::QNodeId textureId);
    void checkInitialized();

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) Q_DECL_FINAL;

    QOpenGLContext *m_context;
    GraphicsContext *m_graphicsContext;
    QThread *m_renderThread;
    Qt3DCore::QNodeId m_textureId;
    QSharedPointer<Scene2DSharedObject> m_sharedObject;
    AttachmentPack m_attachments;
    Texture *m_texture;

    bool m_initialized;
    bool m_renderInitialized;
    bool m_renderOnce;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SCENE2D_P_H
