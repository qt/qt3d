// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "openglvertexarrayobject_p.h"
#include <submissioncontext_p.h>
#include <renderer_p.h>
#include <glresourcemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

OpenGLVertexArrayObject::OpenGLVertexArrayObject()
    : m_ctx(nullptr)
    , m_specified(false)
    , m_supportsVao(false)
{}

void OpenGLVertexArrayObject::bind()
{
    Q_ASSERT(m_ctx);
    if (m_supportsVao) {
        Q_ASSERT(!m_vao.isNull());
        Q_ASSERT(m_vao->isCreated());
        m_vao->bind();
    } else {
        // Unbind any other VAO that may have been bound and not released correctly
        if (m_ctx->m_currentVAO != nullptr && m_ctx->m_currentVAO != this)
            m_ctx->m_currentVAO->release();

        m_ctx->m_currentVAO = this;
        // We need to specify array and vertex attributes
        for (const SubmissionContext::VAOVertexAttribute &attr : m_vertexAttributes)
            m_ctx->enableAttribute(attr);
        if (!m_indexAttribute.isNull())
            m_ctx->bindGLBuffer(m_ctx->m_renderer->glResourceManagers()->glBufferManager()->data(m_indexAttribute),
                                GLBuffer::IndexBuffer);
    }
}

void OpenGLVertexArrayObject::release()
{
    Q_ASSERT(m_ctx);
    if (m_supportsVao) {
        Q_ASSERT(!m_vao.isNull());
        Q_ASSERT(m_vao->isCreated());
        m_vao->release();
    } else {
        if (m_ctx->m_currentVAO == this) {
            for (const SubmissionContext::VAOVertexAttribute &attr : m_vertexAttributes)
                m_ctx->disableAttribute(attr);
            m_ctx->m_currentVAO = nullptr;
        }
    }
}

// called from Render thread
void OpenGLVertexArrayObject::create(SubmissionContext *ctx, const VAOIdentifier &key)
{
    QMutexLocker lock(&m_mutex);

    Q_ASSERT(!m_ctx && !m_vao);

    m_ctx = ctx;
    m_supportsVao = m_ctx->supportsVAO();
    if (m_supportsVao) {
        m_vao.reset(new QOpenGLVertexArrayObject());
        m_vao->create();
    }
    m_owners = key;
}

VAOIdentifier OpenGLVertexArrayObject::key() const
{
    return m_owners;
}

// called from Render thread
void OpenGLVertexArrayObject::destroy()
{
    QMutexLocker locker(&m_mutex);

    Q_ASSERT(m_ctx);
    cleanup();
}

void OpenGLVertexArrayObject::cleanup()
{
    m_vao.reset();
    m_ctx = nullptr;
    m_specified = false;
    m_supportsVao = false;
    m_indexAttribute = SubmissionContext::VAOIndexAttribute();
    m_vertexAttributes.clear();
}

// called from job
bool OpenGLVertexArrayObject::isAbandoned(GeometryManager *geomMgr, GLShaderManager *shaderMgr)
{
    QMutexLocker lock(&m_mutex);

    if (!m_ctx)
        return false;

    const bool geometryExists = (geomMgr->data(m_owners.first) != nullptr);
    const bool shaderExists = (shaderMgr->lookupResource(m_owners.second) != nullptr);

    return !geometryExists || !shaderExists;
}

void OpenGLVertexArrayObject::saveVertexAttribute(const SubmissionContext::VAOVertexAttribute &attr)
{
    // Remove any vertexAttribute already at location
    m_vertexAttributes.erase(std::remove_if(m_vertexAttributes.begin(),
                                            m_vertexAttributes.end(),
                                            [attr](const SubmissionContext::VAOVertexAttribute &a) {
                                                return a.location == attr.location;
                                            }),
                             m_vertexAttributes.end());
    m_vertexAttributes.push_back(attr);
}


} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
