/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERBUFFER_P_H
#define QT3DRENDER_RENDER_OPENGL_RENDERBUFFER_P_H

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
namespace OpenGL {

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

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERBUFFER_P_H
