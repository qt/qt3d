// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_2_H
#define QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_2_H

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

#include <graphicshelperes3_1_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsHelperES3_2 : public GraphicsHelperES3_1
{
public:
    GraphicsHelperES3_2();
    ~GraphicsHelperES3_2();

    bool supportsFeature(Feature feature) const override;

    // QGraphicHelperInterface interface
    void bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment) override;
    bool frameBufferNeedsRenderBuffer(const Attachment &attachment) override;
    void setVerticesPerPatch(GLint verticesPerPatch) override;
    void drawElementsInstancedBaseVertexBaseInstance(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices, GLsizei instances, GLint baseVertex = 0,  GLint baseInstance = 0) override;
    UniformType uniformTypeFromGLType(GLenum glType) override;
    uint uniformByteSize(const ShaderUniform &description) override;

};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_2_H
