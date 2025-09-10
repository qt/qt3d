// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RENDERBUFFER_P_H
#define QT3DRENDER_RENDER_RHI_RENDERBUFFER_P_H

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

#include <Qt3DRender/qabstracttexture.h>

QT_BEGIN_NAMESPACE

class QOpenGLContext;

namespace Qt3DRender {
namespace Render {
namespace Rhi {

class Q_AUTOTEST_EXPORT RenderBuffer
{
public:
    RenderBuffer(int width, int height, QAbstractTexture::TextureFormat format);
    ~RenderBuffer();

    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }
    QSize size() const { return m_size; }
    QAbstractTexture::TextureFormat format() const { return m_format; }
    GLuint renderBufferId() const { return m_renderBuffer; }

    void bind();
    void release();

private:
    QSize m_size;
    QAbstractTexture::TextureFormat m_format;
    GLuint m_renderBuffer;
    QOpenGLContext *m_context;
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERBUFFER_P_H
