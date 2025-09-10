// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderbuffer_p.h"
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

RenderBuffer::RenderBuffer(int width, int height, QAbstractTexture::TextureFormat format)
    : m_size(width, height), m_format(format), m_renderBuffer(0), m_context(nullptr)
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning("Renderbuffer requires an OpenGL context");
        return;
    }

    m_context = ctx;
    QOpenGLFunctions *f = ctx->functions();
    f->glGenRenderbuffers(1, &m_renderBuffer);
    if (!m_renderBuffer)
        return;

    f->glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    while (f->glGetError() != GL_NO_ERROR) { }
    f->glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    GLint err = f->glGetError();
    if (err != GL_NO_ERROR)
        qWarning("Failed to set renderbuffer storage: error 0x%x", err);
    f->glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::~RenderBuffer()
{
    if (m_renderBuffer) {
        QOpenGLContext *ctx = QOpenGLContext::currentContext();

        // Ignore the fact that renderbuffers are sharable resources and let's
        // just expect that the context is the same as when the resource was
        // created. QOpenGLTexture suffers from the same limitation anyway, and
        // this is unlikely to become an issue within Qt 3D.
        if (ctx == m_context) {
            ctx->functions()->glDeleteRenderbuffers(1, &m_renderBuffer);
        } else {
            qWarning("Wrong current context; renderbuffer not destroyed");
        }
    }
}

void RenderBuffer::bind()
{
    if (!m_renderBuffer)
        return;

    m_context->functions()->glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
}

void RenderBuffer::release()
{
    if (!m_context)
        return;

    m_context->functions()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
