// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H
#define QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H

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
#include <rhihandle_types_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <rhi/qrhi.h>
#include <Qt3DCore/qattribute.h>
#include <variant>

QT_BEGIN_NAMESPACE
class QRhiGraphicsPipeline;
class QRhiShaderResourceBindings;

namespace Qt3DRender {

namespace Render {

class RenderStateSet;
using RenderStateSetPtr = QSharedPointer<RenderStateSet>;

namespace Rhi {

class RHIShader;
class RHIGraphicsPipeline;
class RHIComputePipeline;

struct CommandUBO
{
    float modelMatrix[16];
    float inverseModelMatrix[16];
    float modelViewMatrix[16];
    float modelNormalMatrix[12];
    float inverseModelViewMatrix[16];
    float mvp[16];
    float inverseModelViewProjectionMatrix[16];
    float modelViewNormalMatrix[12];
    float skinningPalette[100 * 16];
};
static_assert(sizeof(CommandUBO) == 6 * (16 * sizeof(float)) + 2 * (12 * sizeof(float)) + 100 * (16 * sizeof(float)),
              "UBO doesn't match std140");

struct Q_AUTOTEST_EXPORT AttributeInfo
{
    int nameId = -1;
    QRhiVertexInputBinding::Classification classification = QRhiVertexInputBinding::PerVertex;
    size_t stride = 0;
    size_t offset = 0;
    size_t divisor = 0;
};

Q_AUTOTEST_EXPORT bool operator==(const AttributeInfo &a, const AttributeInfo &b);
Q_AUTOTEST_EXPORT bool operator!=(const AttributeInfo &a, const AttributeInfo &b);

class Q_AUTOTEST_EXPORT RenderCommand
{
public:
    RenderCommand();
    ~RenderCommand();

    bool isValid() const noexcept;

    HMaterial m_material; // Purely used to ease sorting (minimize stage changes, binding changes
                          // ....)
    RHIShader *m_rhiShader; // Shader to be used at render time
    Qt3DCore::QNodeId m_shaderId; // Shader for given pass and mesh
    ShaderParameterPack m_parameterPack; // Might need to be reworked so as to be able to destroy
                                         // the Texture while submission is happening.
    RenderStateSetPtr m_stateSet;

    HGeometry m_geometry;
    HGeometryRenderer m_geometryRenderer;

    HBuffer m_indirectDrawBuffer; // Reference to indirect draw buffer (valid only m_drawIndirect ==
                                  // true)
    HComputeCommand m_computeCommand;

    // A QAttribute pack might be interesting
    // This is a temporary fix in the meantime, to remove the hacked methods in Technique
    std::vector<int> m_activeAttributes;

    float m_depth;
    int m_changeCost;

    enum CommandType { Draw, Compute };

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
    Qt3DCore::QAttribute::VertexBaseType m_indexAttributeDataType;
    uint m_indirectAttributeByteOffset;
    bool m_drawIndexed;
    bool m_drawIndirect;
    bool m_primitiveRestartEnabled;
    bool m_isValid;

    std::vector<AttributeInfo> m_attributeInfo;
    QVarLengthArray<QRhiCommandBuffer::VertexInput, 8> vertex_input;

    const Attribute *indexAttribute {};
    QRhiBuffer *indexBuffer {};

    CommandUBO m_commandUBO;
    QRhiShaderResourceBindings *shaderResourceBindings = nullptr;
    std::vector<QRhiShaderResourceBinding> resourcesBindings;

    struct Pipeline : std::variant<std::monostate, RHIGraphicsPipeline *, RHIComputePipeline*>
    {
        using variant::variant;

        bool isValid() const noexcept;

        RHIGraphicsPipeline* graphics() const noexcept
        {
            auto ptr = std::get_if<RHIGraphicsPipeline*>(this);
            return ptr ? *ptr : nullptr;
        }

        RHIComputePipeline* compute() const noexcept
        {
            auto ptr = std::get_if<RHIComputePipeline*>(this);
            return ptr ? *ptr : nullptr;
        }

        template<typename F>
        auto visit(F&& f) const
        {
            return std::visit(f, (const variant&) *this);
        }
    };

    Pipeline pipeline {};
};

Q_AUTOTEST_EXPORT bool operator==(const RenderCommand &a, const RenderCommand &b) noexcept;

inline bool operator!=(const RenderCommand &lhs, const RenderCommand &rhs) noexcept
{
    return !operator==(lhs, rhs);
}

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H
