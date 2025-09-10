// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "shaderparameterpack_p.h"

#include <submissioncontext_p.h>
#include <Qt3DRender/private/texture_p.h>

#include <Qt3DCore/private/vector_helper_p.h>

#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QColor>
#include <QQuaternion>
#include <Qt3DRender/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

ShaderParameterPack::~ShaderParameterPack()
{
}

void ShaderParameterPack::reserve(int uniformCount)
{
    m_uniforms.reserve(uniformCount);
    m_submissionUniformIndices.reserve(uniformCount);
}

void ShaderParameterPack::setUniform(const int glslNameId, const UniformValue &val)
{
    m_uniforms.insert(glslNameId, val);
}

void ShaderParameterPack::setTexture(const int glslNameId, int uniformArrayIndex, Qt3DCore::QNodeId texId)
{
    for (size_t t = 0; t < m_textures.size(); ++t) {
        if (m_textures[t].glslNameId != glslNameId || m_textures[t].uniformArrayIndex != uniformArrayIndex)
            continue;

        m_textures[t].nodeId = texId;
        return;
    }

    m_textures.push_back(NamedResource(glslNameId, texId, uniformArrayIndex, NamedResource::Texture));
}

void ShaderParameterPack::setImage(const int glslNameId, int uniformArrayIndex, Qt3DCore::QNodeId id)
{
    for (size_t i=0, m = m_images.size(); i < m; ++i) {
        if (m_images[i].glslNameId != glslNameId || m_images[i].uniformArrayIndex != uniformArrayIndex)
            continue;

        m_images[i].nodeId = id;
        return;
    }

    m_images.push_back(NamedResource(glslNameId, id, uniformArrayIndex, NamedResource::Image));
}

// Contains Uniform Block Index and QNodeId of the ShaderData (UBO)
void ShaderParameterPack::setUniformBuffer(BlockToUBO blockToUBO)
{
    m_uniformBuffers.push_back(std::move(blockToUBO));
}

void ShaderParameterPack::setShaderStorageBuffer(BlockToSSBO blockToSSBO)
{
    m_shaderStorageBuffers.push_back(std::move(blockToSSBO));
}

void ShaderParameterPack::setShaderDataForUBO(ShaderDataForUBO shaderDataForUBO)
{
    if (!Qt3DCore::contains(m_shaderDatasForUBOs, shaderDataForUBO))
        m_shaderDatasForUBOs.push_back(std::move(shaderDataForUBO));
}

void ShaderParameterPack::setSubmissionUniformIndex(const int uniformIdx)
{
    m_submissionUniformIndices.push_back(uniformIdx);
}

bool operator==(const ShaderDataForUBO &a, const ShaderDataForUBO &b)
{
    return a.m_bindingIndex == b.m_bindingIndex && a.m_shaderDataID == b.m_shaderDataID;
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
