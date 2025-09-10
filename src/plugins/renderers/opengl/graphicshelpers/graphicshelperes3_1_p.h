// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_GRAPHICSHELPERES3_1_H
#define QT3DRENDER_RENDER_GRAPHICSHELPERES3_1_H

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

#include <graphicshelperes3_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsHelperES3_1 : public GraphicsHelperES3
{
public:
    GraphicsHelperES3_1();
    ~GraphicsHelperES3_1();

    bool supportsFeature(Feature feature) const override;
    void bindImageTexture(GLuint imageUnit, GLuint texture, GLint mipLevel, GLboolean layered, GLint layer, GLenum access, GLenum format) override;
    void dispatchCompute(GLuint wx, GLuint wy, GLuint wz) override;
    void memoryBarrier(QMemoryBarrier::Operations barriers) override;
    void drawArraysIndirect(GLenum mode,void *indirect) override;
    void drawElementsIndirect(GLenum mode, GLenum type, void *indirect) override;
    void bindShaderStorageBlock(GLuint programId, GLuint shaderStorageBlockIndex, GLuint shaderStorageBlockBinding) override;
    std::vector<ShaderStorageBlock> programShaderStorageBlocks(GLuint programId) override;

    // QGraphicHelperInterface interface
    UniformType uniformTypeFromGLType(GLenum glType) override;
    uint uniformByteSize(const ShaderUniform &description) override;
    void buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer) override;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GRAPHICSHELPERES3_1_H
