// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RESOURCEACCESSOR_P_H
#define QT3DRENDER_RENDER_RESOURCEACCESSOR_P_H

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

#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

class QMutex;

namespace Qt3DRender
{
namespace Render {

class TextureManager;
class AttachmentManager;
class GLTextureManager;
class EntityManager;
class NodeManagers;
class AbstractRenderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderBackendResourceAccessor
{
public:
    enum ResourceType {
        OGLTextureWrite,
        OGLTextureRead,
        OutputAttachment,
        EntityHandle,
    };

    virtual ~RenderBackendResourceAccessor();
    virtual bool accessResource(ResourceType type, Qt3DCore::QNodeId nodeId, void **handle, QMutex **lock) = 0;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT ResourceAccessor : public RenderBackendResourceAccessor
{
public:
    ResourceAccessor(AbstractRenderer *renderer, NodeManagers *mgr);
    bool accessResource(ResourceType type, Qt3DCore::QNodeId nodeId, void **handle, QMutex **lock) final;
private:
    AbstractRenderer *m_renderer;
    TextureManager *m_textureManager;
    AttachmentManager *m_attachmentManager;
    EntityManager *m_entityManager;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RESOURCEACCESSOR_P_H
