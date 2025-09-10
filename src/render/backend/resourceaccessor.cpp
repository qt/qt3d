// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "resourceaccessor_p.h"

#include <Qt3DRender/qrendertargetoutput.h>

#include <private/qrendertargetoutput_p.h>
#include <private/nodemanagers_p.h>
#include <private/rendertargetoutput_p.h>
#include <private/managers_p.h>

#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

RenderBackendResourceAccessor::~RenderBackendResourceAccessor()
{

}

ResourceAccessor::ResourceAccessor(AbstractRenderer *renderer, NodeManagers *mgr)
    : m_renderer(renderer)
    , m_textureManager(mgr->textureManager())
    , m_attachmentManager(mgr->attachmentManager())
    , m_entityManager(mgr->renderNodesManager())
{

}

// called by render plugins from arbitrary thread
bool ResourceAccessor::accessResource(ResourceType type,
                                      Qt3DCore::QNodeId nodeId,
                                      void **handle,
                                      QMutex **lock)
{
    switch (type) {

    // This is purely made so that Scene2D works, this should be completely
    // redesigned to avoid introducing this kind of coupling and reliance on
    // OpenGL
    case RenderBackendResourceAccessor::OGLTextureWrite:
        Q_FALLTHROUGH();
    case RenderBackendResourceAccessor::OGLTextureRead:
    {
        if (m_renderer->api() != API::OpenGL) {
            qWarning() << "Renderer plugin is not compatible with Scene2D";
            return false;
        }
        return m_renderer->accessOpenGLTexture(nodeId,
                                               reinterpret_cast<QOpenGLTexture **>(handle),
                                               lock,
                                               type == RenderBackendResourceAccessor::OGLTextureRead);
    }

    case RenderBackendResourceAccessor::OutputAttachment: {
        RenderTargetOutput *output = m_attachmentManager->lookupResource(nodeId);
        if (output) {
            Attachment **attachmentData = reinterpret_cast<Attachment **>(handle);
            *attachmentData = output->attachment();
            return true;
        }
        break;
    }

    case RenderBackendResourceAccessor::EntityHandle: {
        Entity *entity = m_entityManager->lookupResource(nodeId);
        if (entity) {
            Entity **pEntity = reinterpret_cast<Entity **>(handle);
            *pEntity = entity;
            return true;
        }
        break;
    }

    default:
        break;
    }
    return false;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
