/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#include <submissioncontext_p.h>
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
namespace Rhi {

ShaderParameterPack::~ShaderParameterPack() { }

void ShaderParameterPack::setUniform(const int glslNameId, const UniformValue &val)
{
    m_uniforms.insert(glslNameId, val);
}

void ShaderParameterPack::setTexture(const int glslNameId, int uniformArrayIndex,
                                     Qt3DCore::QNodeId texId)
{
    for (NamedResource &texture : m_textures) {
        if (texture.glslNameId != glslNameId || texture.uniformArrayIndex != uniformArrayIndex)
            continue;

        texture.nodeId = texId;
        return;
    }

    m_textures.append(NamedResource(glslNameId, texId, uniformArrayIndex, NamedResource::Texture));
}

void ShaderParameterPack::setImage(const int glslNameId, int uniformArrayIndex,
                                   Qt3DCore::QNodeId id)
{
    for (NamedResource &image : m_images) {
        if (image.glslNameId != glslNameId || image.uniformArrayIndex != uniformArrayIndex)
            continue;

        image.nodeId = id;
        return;
    }

    m_images.append(NamedResource(glslNameId, id, uniformArrayIndex, NamedResource::Image));
}

// Contains Uniform Block Index and QNodeId of the ShaderData (UBO)
void ShaderParameterPack::setUniformBuffer(BlockToUBO blockToUBO)
{
    m_uniformBuffers.append(std::move(blockToUBO));
}

void ShaderParameterPack::setShaderStorageBuffer(BlockToSSBO blockToSSBO)
{
    m_shaderStorageBuffers.push_back(blockToSSBO);
}

void ShaderParameterPack::setSubmissionUniform(const ShaderUniform &uniform)
{
    m_submissionUniforms.push_back(uniform);
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
