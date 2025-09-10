// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "glbuffer_p.h"
#include <graphicscontext_p.h>

#if !defined(GL_UNIFORM_BUFFER)
#define GL_UNIFORM_BUFFER 0x8A11
#endif
#if !defined(GL_ARRAY_BUFFER)
#define GL_ARRAY_BUFFER 0x8892
#endif
#if !defined(GL_ELEMENT_ARRAY_BUFFER)
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#if !defined(GL_SHADER_STORAGE_BUFFER)
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#endif
#if !defined(GL_PIXEL_PACK_BUFFER)
#define GL_PIXEL_PACK_BUFFER 0x88EB
#endif
#if !defined(GL_PIXEL_UNPACK_BUFFER)
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#endif
#if !defined(GL_DRAW_INDIRECT_BUFFER)
#define GL_DRAW_INDIRECT_BUFFER 0x8F3F
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

// A UBO is created for each ShaderData Shader Pair
// That means a UBO is unique to a shader/shaderdata

namespace {

GLenum glBufferTypes[] = {
    GL_ARRAY_BUFFER,
    GL_UNIFORM_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_DRAW_INDIRECT_BUFFER
};

} // anonymous

GLBuffer::GLBuffer()
    : m_bufferId(0)
    , m_isCreated(false)
    , m_bound(false)
    , m_lastTarget(GL_ARRAY_BUFFER)
{
}

bool GLBuffer::bind(GraphicsContext *ctx, Type t)
{
    if (m_bufferId == 0)
        return false;
    m_lastTarget = glBufferTypes[t];
    ctx->openGLContext()->functions()->glBindBuffer(m_lastTarget, m_bufferId);
    m_bound = true;
    return true;
}

bool GLBuffer::release(GraphicsContext *ctx)
{
    m_bound = false;
    ctx->openGLContext()->functions()->glBindBuffer(m_lastTarget, 0);
    return true;
}

bool GLBuffer::create(GraphicsContext *ctx)
{
    ctx->openGLContext()->functions()->glGenBuffers(1, &m_bufferId);
    m_isCreated = true;
    return m_bufferId != 0;
}

void GLBuffer::destroy(GraphicsContext *ctx)
{
    ctx->openGLContext()->functions()->glDeleteBuffers(1, &m_bufferId);
    m_isCreated = false;
}

void GLBuffer::allocate(GraphicsContext *ctx, uint size, bool dynamic)
{
    // Either GL_STATIC_DRAW OR GL_DYNAMIC_DRAW depending on  the use case
    // TO DO: find a way to know how a buffer/QShaderData will be used to use the right usage
    ctx->openGLContext()->functions()->glBufferData(m_lastTarget, size, NULL, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void GLBuffer::allocate(GraphicsContext *ctx, const void *data, uint size, bool dynamic)
{
    ctx->openGLContext()->functions()->glBufferData(m_lastTarget, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void GLBuffer::update(GraphicsContext *ctx, const void *data, uint size, int offset)
{
    ctx->openGLContext()->functions()->glBufferSubData(m_lastTarget, offset, size, data);
}

QByteArray GLBuffer::download(GraphicsContext *ctx, uint size)
{
    char *gpu_ptr = ctx->mapBuffer(m_lastTarget, size);
    QByteArray data;
    if (gpu_ptr != nullptr) {
        data.resize(size);
        std::copy(gpu_ptr, gpu_ptr+size, data.data());
    }
    ctx->unmapBuffer(m_lastTarget);
    return data;
}

void GLBuffer::bindBufferBase(GraphicsContext *ctx, int bindingPoint, GLBuffer::Type t)
{
    ctx->bindBufferBase(glBufferTypes[t], bindingPoint, m_bufferId);
}

void GLBuffer::bindBufferBase(GraphicsContext *ctx, int bindingPoint)
{
    ctx->bindBufferBase(m_lastTarget, bindingPoint, m_bufferId);
}

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
