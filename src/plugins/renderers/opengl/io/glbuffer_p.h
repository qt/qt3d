// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_GLBUFFER_P_H
#define QT3DRENDER_RENDER_OPENGL_GLBUFFER_P_H

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

#include <QOpenGLContext>
#include <Qt3DCore/qnodeid.h>
#include <qbytearray.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

class GraphicsContext;

class GLBuffer
{
public:
    GLBuffer();

    enum Type
    {
        ArrayBuffer = 0,
        UniformBuffer,
        IndexBuffer,
        ShaderStorageBuffer,
        PixelPackBuffer,
        PixelUnpackBuffer,
        DrawIndirectBuffer
    };

    bool bind(GraphicsContext *ctx, Type t);
    bool release(GraphicsContext *ctx);
    bool create(GraphicsContext *ctx);
    void destroy(GraphicsContext *ctx);
    void allocate(GraphicsContext *ctx, uint size, bool dynamic = true);
    void allocate(GraphicsContext *ctx, const void *data, uint size, bool dynamic = true);
    void update(GraphicsContext *ctx, const void *data, uint size, int offset = 0);
    QByteArray download(GraphicsContext *ctx, uint size);
    void bindBufferBase(GraphicsContext *ctx, int bindingPoint, Type t);
    void bindBufferBase(GraphicsContext *ctx, int bindingPoint);

    inline GLuint bufferId() const { return m_bufferId; }
    inline bool isCreated() const { return m_isCreated; }
    inline bool isBound() const { return m_bound; }

private:
    GLuint m_bufferId;
    bool m_isCreated;
    bool m_bound;
    GLenum m_lastTarget;
};

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GLBUFFER_P_H
