// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERCOMMAND_H
#define QT3DRENDER_RENDER_OPENGL_RENDERCOMMAND_H

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

#include <qglobal.h>
#include <shaderparameterpack_p.h>
#include <gl_handle_types_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <vector>

QT_BEGIN_NAMESPACE

class QOpenGLVertexArrayObject;

namespace Qt3DRender {

namespace Render {

class RenderStateSet;
using RenderStateSetPtr = QSharedPointer<RenderStateSet>;

namespace OpenGL {

class GLShader;

class Q_AUTOTEST_EXPORT RenderCommand
{
public:
    RenderCommand();

    HVao m_vao; // VAO used during the submission step to store all states and VBOs
    HMaterial m_material; // Purely used to ease sorting (minimize stage changes, binding changes ....)
    GLShader *m_glShader; // GL Shader to be used at render time
    Qt3DCore::QNodeId m_shaderId; // Shader for given pass and mesh
    ShaderParameterPack m_parameterPack; // Might need to be reworked so as to be able to destroy the
                            // Texture while submission is happening.
    RenderStateSetPtr m_stateSet;

    HGeometry m_geometry;
    HGeometryRenderer m_geometryRenderer;

    HBuffer m_indirectDrawBuffer; // Reference to indirect draw buffer (valid only m_drawIndirect == true)
    HComputeCommand m_computeCommand;

    // A QAttribute pack might be interesting
    // This is a temporary fix in the meantime, to remove the hacked methods in Technique
    std::vector<int> m_activeAttributes;

    float m_depth;
    int m_changeCost;

    enum CommandType {
        Draw,
        Compute
    };

    CommandType m_type;
    int m_workGroups[3];

    // Values filled for draw calls by Renderer (in prepare Submission)
    GLsizei m_primitiveCount;
    QGeometryRenderer::PrimitiveType m_primitiveType;
    int m_restartIndexValue;
    int m_firstInstance;
    int m_firstVertex;
    int m_verticesPerPatch;
    int m_instanceCount;
    int m_indexOffset;
    uint m_indexAttributeByteOffset;
    GLint m_indexAttributeDataType;
    uint m_indirectAttributeByteOffset;
    bool m_drawIndexed;
    bool m_drawIndirect;
    bool m_primitiveRestartEnabled;
    bool m_isValid;
};

Q_AUTOTEST_EXPORT bool operator==(const RenderCommand &a, const RenderCommand &b) noexcept;

inline bool operator!=(const RenderCommand &lhs, const RenderCommand &rhs) noexcept
{ return !operator==(lhs, rhs); }

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERCOMMAND_H
