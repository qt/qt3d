/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include "shaderparameterpack_p.h"

#include <graphicscontext_p.h>
#include <Qt3DRender/private/texture_p.h>

#include <Qt3DCore/private/qframeallocator_p.h>

#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QColor>
#include <QQuaternion>
#include <Qt3DRender/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

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
    for (int t=0; t<m_textures.size(); ++t) {
        if (m_textures[t].glslNameId != glslNameId || m_textures[t].uniformArrayIndex != uniformArrayIndex)
            continue;

        m_textures[t].nodeId = texId;
        return;
    }

    m_textures.push_back(NamedResource(glslNameId, texId, uniformArrayIndex, NamedResource::Texture));
}

void ShaderParameterPack::setImage(const int glslNameId, int uniformArrayIndex, Qt3DCore::QNodeId id)
{
    for (int i=0, m = m_images.size(); i < m; ++i) {
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
    const auto uEnd = m_uniformBuffers.end();
    auto it = std::find_if(m_uniformBuffers.begin(), uEnd, [&] (const BlockToUBO &block) {
        return blockToUBO.m_blockIndex == block.m_blockIndex;
    });

    if (it == uEnd)
        m_uniformBuffers.push_back(std::move(blockToUBO));
    else
        *it = std::move(blockToUBO);
}

void ShaderParameterPack::setShaderStorageBuffer(BlockToSSBO blockToSSBO)
{
    const auto uEnd = m_shaderStorageBuffers.end();
    auto it = std::find_if(m_shaderStorageBuffers.begin(), uEnd, [&] (const BlockToSSBO &block) {
        return blockToSSBO.m_blockIndex == block.m_blockIndex;
    });

    if (it == uEnd)
        m_shaderStorageBuffers.push_back(std::move(blockToSSBO));
    else
        *it = std::move(blockToSSBO);
}

void ShaderParameterPack::setSubmissionUniformIndex(const int uniformIdx)
{
    m_submissionUniformIndices.push_back(uniformIdx);
}

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
