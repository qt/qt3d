/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "shader_p.h"
#include <QMutexLocker>
#include <Qt3DRender/private/stringtoint_p.h>
#include <graphicscontext_p.h>
#include <logging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RHIShader::RHIShader()
    : m_isLoaded(false)
    , m_graphicsContext(nullptr)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

void RHIShader::setGraphicsContext(GraphicsContext *context)
{
    QMutexLocker lock(&m_mutex);
    m_graphicsContext = context;
    // if (m_graphicsContext) {
    //     m_contextConnection = QObject::connect(m_graphicsContext->openGLContext(),
    //                                            &QOpenGLContext::aboutToBeDestroyed,
    //                                            [this] { setGraphicsContext(nullptr); });
    // }
}

GraphicsContext *RHIShader::graphicsContext()
{
    QMutexLocker lock(&m_mutex);
    return m_graphicsContext;
}


QVector<QString> RHIShader::uniformsNames() const
{
    return m_uniformsNames;
}

QVector<QString> RHIShader::attributesNames() const
{
    return m_attributesNames;
}

QVector<QString> RHIShader::uniformBlockNames() const
{
    return m_uniformBlockNames;
}

QVector<QString> RHIShader::storageBlockNames() const
{
    return m_shaderStorageBlockNames;
}

QVector<QByteArray> RHIShader::shaderCode() const
{
    return m_shaderCode;
}

static auto rhiInputType(QShaderDescription::VariableType type)
{
    switch(type)
    {
    case QShaderDescription::Vec4: return QRhiVertexInputAttribute::Float4;
    case QShaderDescription::Vec3: return QRhiVertexInputAttribute::Float3;
    case QShaderDescription::Vec2: return QRhiVertexInputAttribute::Float2;
    case QShaderDescription::Float: return QRhiVertexInputAttribute::Float;
    default:
        // TODO UNormByte4, UNormByte2, UNormByte
        RHI_UNIMPLEMENTED;
        return QRhiVertexInputAttribute::UNormByte;
        break;
    }
}

static constexpr int rhiTypeSize(QShaderDescription::VariableType type)
{
    switch(type)
    {
    case QShaderDescription::Unknown: return 0;

    case QShaderDescription::Float: return 1;
    case QShaderDescription::Vec2: return 2;
    case QShaderDescription::Vec3: return 3;
    case QShaderDescription::Vec4: return 4;
    case QShaderDescription::Mat2: return 2*2;
    case QShaderDescription::Mat2x3: return 2*3;
    case QShaderDescription::Mat2x4: return 2*4;
    case QShaderDescription::Mat3: return 3*3;
    case QShaderDescription::Mat3x2: return 3*2;
    case QShaderDescription::Mat3x4: return 3*4;
    case QShaderDescription::Mat4: return 4*4;
    case QShaderDescription::Mat4x2: return 4*2;
    case QShaderDescription::Mat4x3: return 4*3;

    case QShaderDescription::Int: return 1;
    case QShaderDescription::Int2: return 2;
    case QShaderDescription::Int3: return 3;
    case QShaderDescription::Int4: return 4;

    case QShaderDescription::Uint: return 1;
    case QShaderDescription::Uint2: return 2;
    case QShaderDescription::Uint3: return 3;
    case QShaderDescription::Uint4: return 4;

    case QShaderDescription::Bool: return 1;
    case QShaderDescription::Bool2: return 2;
    case QShaderDescription::Bool3: return 3;
    case QShaderDescription::Bool4: return 4;

    case QShaderDescription::Double: return 1;
    case QShaderDescription::Double2: return 2;
    case QShaderDescription::Double3: return 3;
    case QShaderDescription::Double4: return 4;
    case QShaderDescription::DMat2: return 2*2;
    case QShaderDescription::DMat2x3: return 2*3;
    case QShaderDescription::DMat2x4: return 2*4;
    case QShaderDescription::DMat3: return 3*3;
    case QShaderDescription::DMat3x2: return 3*2;
    case QShaderDescription::DMat3x4: return 3*4;
    case QShaderDescription::DMat4: return 4*4;
    case QShaderDescription::DMat4x2: return 4*2;
    case QShaderDescription::DMat4x3: return 4*3;

    case QShaderDescription::Sampler1D: return 0;
    case QShaderDescription::Sampler2D: return 0;
    case QShaderDescription::Sampler2DMS: return 0;
    case QShaderDescription::Sampler3D: return 0;
    case QShaderDescription::SamplerCube: return 0;
    case QShaderDescription::Sampler1DArray: return 0;
    case QShaderDescription::Sampler2DArray: return 0;
    case QShaderDescription::Sampler2DMSArray: return 0;
    case QShaderDescription::Sampler3DArray: return 0;
    case QShaderDescription::SamplerCubeArray: return 0;
    case QShaderDescription::SamplerRect: return 0;
    case QShaderDescription::SamplerBuffer: return 0;

    case QShaderDescription::Image1D: return 0;
    case QShaderDescription::Image2D: return 0;
    case QShaderDescription::Image2DMS: return 0;
    case QShaderDescription::Image3D: return 0;
    case QShaderDescription::ImageCube: return 0;
    case QShaderDescription::Image1DArray: return 0;
    case QShaderDescription::Image2DArray: return 0;
    case QShaderDescription::Image2DMSArray: return 0;
    case QShaderDescription::Image3DArray: return 0;
    case QShaderDescription::ImageCubeArray: return 0;
    case QShaderDescription::ImageRect: return 0;
    case QShaderDescription::ImageBuffer: return 0;

    case QShaderDescription::Struct: return 0;
    default: return 0;
    }
}

template<typename T, typename Pred>
auto stableRemoveDuplicates(QVector<T> in, Pred predicate)
{
    QVector<T> out;
    for(const auto& element : in)
    {
        if(std::none_of(out.begin(), out.end(), [&] (auto& other) { return predicate(element, other); }))
            out.push_back(element);
    }
    return out;
}
void RHIShader::introspect()
{
    QVector<QShaderDescription::UniformBlock> rhiUBO;
    QVector<QShaderDescription::StorageBlock> rhiSSBO;

    QVector<ShaderUniformBlock> uniformBlocks;
    QVector<ShaderStorageBlock> storageBlocks;
    QVector<ShaderAttribute> attributes;

    QRhiVertexInputLayout lay;

    // Introspect shader vertex input
    if(m_stages[QShader::VertexStage].isValid())
    {
        const QShaderDescription& vtx = m_stages[QShader::VertexStage].description();

        QVarLengthArray<QRhiVertexInputAttribute, 8> rhiAttributes;

        for(const QShaderDescription::InOutVariable& input : vtx.inputVariables())
        {
            // TODO offset ?
            rhiAttributes.push_back({ 0, input.location, rhiInputType(input.type), 0 });
            qDebug() << "Pushing attribute: " << rhiAttributes.back();

            attributes.push_back(ShaderAttribute{input.name, -1, input.type, rhiTypeSize(input.type), input.location});
        }
        lay.setAttributes(rhiAttributes.begin(), rhiAttributes.end());

        for(const auto& var: vtx.uniformBlocks())
            rhiUBO.push_back(var);
        for(const auto& var: vtx.storageBlocks())
            rhiSSBO.push_back(var);
    }
    m_input = std::move(lay);

    // Introspect shader uniforms

    if(m_stages[QShader::FragmentStage].isValid())
    {
        const QShaderDescription& frag = m_stages[QShader::FragmentStage].description();
        for(const auto& var: frag.uniformBlocks())
            rhiUBO.push_back(var);
        for(const auto& var: frag.storageBlocks())
            rhiSSBO.push_back(var);
    }

    rhiUBO = stableRemoveDuplicates(rhiUBO, [] (const QShaderDescription::UniformBlock& lhs, const QShaderDescription::UniformBlock& rhs) {
       return lhs.blockName == rhs.blockName;
    });
    rhiSSBO = stableRemoveDuplicates(rhiSSBO, [] (const QShaderDescription::StorageBlock& lhs, const QShaderDescription::StorageBlock& rhs) {
        return lhs.blockName == rhs.blockName;
    });

    for(const QShaderDescription::UniformBlock& ubo : rhiUBO)
    {
        uniformBlocks.push_back(ShaderUniformBlock{ubo.blockName, -1, -1, ubo.binding, 0, 0});
    }

    for(const QShaderDescription::StorageBlock& ssbo : rhiSSBO)
    {
        storageBlocks.push_back(ShaderStorageBlock{ssbo.blockName, -1, -1, ssbo.binding, 0, 0});
    }

    initializeAttributes(attributes);
    initializeUniformBlocks(uniformBlocks);
    initializeShaderStorageBlocks(storageBlocks);
}

QHash<QString, ShaderUniform> RHIShader::activeUniformsForUniformBlock(int blockIndex) const
{
    return m_uniformBlockIndexToShaderUniforms.value(blockIndex);
}

ShaderUniformBlock RHIShader::uniformBlockForBlockIndex(int blockIndex)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_index == blockIndex) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock RHIShader::uniformBlockForBlockNameId(int blockNameId)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_nameId == blockNameId) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock RHIShader::uniformBlockForBlockName(const QString &blockName)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_name == blockName) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockIndex(int blockIndex)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_index == blockIndex)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockNameId(int blockNameId)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_nameId == blockNameId)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockName(const QString &blockName)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_name == blockName)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

void RHIShader::prepareUniforms(ShaderParameterPack &pack)
{
    const PackUniformHash &values = pack.uniforms();

    auto it = values.keys.cbegin();
    const auto end = values.keys.cend();

    while (it != end) {
        // Find if there's a uniform with the same name id
        for (const ShaderUniform &uniform : qAsConst(m_uniforms)) {
            if (uniform.m_nameId == *it) {
                pack.setSubmissionUniform(uniform);
                break;
            }
        }
        ++it;
    }
}

void RHIShader::setFragOutputs(const QHash<QString, int> &fragOutputs)
{
    {
        QMutexLocker lock(&m_mutex);
        m_fragOutputs = fragOutputs;
    }
//    updateDNA();
}

const QHash<QString, int> RHIShader::fragOutputs() const
{
    QMutexLocker lock(&m_mutex);
    return m_fragOutputs;
}

void RHIShader::initializeUniforms(const QVector<ShaderUniform> &uniformsDescription)
{
    m_uniforms = uniformsDescription;
    m_uniformsNames.resize(uniformsDescription.size());
    m_uniformsNamesIds.reserve(uniformsDescription.size());
    m_standardUniformNamesIds.reserve(5);
    QHash<QString, ShaderUniform> activeUniformsInDefaultBlock;

    static const QVector<int> standardUniformNameIds = {
        Shader::modelMatrixNameId,
        Shader::viewMatrixNameId,
        Shader::projectionMatrixNameId,
        Shader::modelViewMatrixNameId,
        Shader::viewProjectionMatrixNameId,
        Shader::modelViewProjectionNameId,
        Shader::mvpNameId,
        Shader::inverseModelMatrixNameId,
        Shader::inverseViewMatrixNameId,
        Shader::inverseProjectionMatrixNameId,
        Shader::inverseModelViewNameId,
        Shader::inverseViewProjectionMatrixNameId,
        Shader::inverseModelViewProjectionNameId,
        Shader::modelNormalMatrixNameId,
        Shader::modelViewNormalNameId,
        Shader::viewportMatrixNameId,
        Shader::inverseViewportMatrixNameId,
        Shader::aspectRatioNameId,
        Shader::exposureNameId,
        Shader::gammaNameId,
        Shader::timeNameId,
        Shader::eyePositionNameId,
        Shader::skinningPaletteNameId,
    };

    for (int i = 0, m = uniformsDescription.size(); i < m; i++) {
        m_uniformsNames[i] = m_uniforms[i].m_name;
        const int nameId = StringToInt::lookupId(m_uniformsNames[i]);
        m_uniforms[i].m_nameId = nameId;

        // Is the uniform a Qt3D "Standard" uniform or a user defined one?
        if (standardUniformNameIds.contains(nameId))
            m_standardUniformNamesIds.push_back(nameId);
        else
            m_uniformsNamesIds.push_back(nameId);

        if (uniformsDescription[i].m_blockIndex == -1) { // Uniform is in default block
            qCDebug(Shaders) << "Active Uniform in Default Block " << uniformsDescription[i].m_name << uniformsDescription[i].m_blockIndex;
            activeUniformsInDefaultBlock.insert(uniformsDescription[i].m_name, uniformsDescription[i]);
        }
    }
    m_uniformBlockIndexToShaderUniforms.insert(-1, activeUniformsInDefaultBlock);
}

void RHIShader::initializeAttributes(const QVector<ShaderAttribute> &attributesDescription)
{
    m_attributes = attributesDescription;
    m_attributesNames.resize(attributesDescription.size());
    m_attributeNamesIds.resize(attributesDescription.size());
    for (int i = 0, m = attributesDescription.size(); i < m; i++) {
        m_attributesNames[i] = attributesDescription[i].m_name;
        m_attributes[i].m_nameId = StringToInt::lookupId(m_attributesNames[i]);
        m_attributeNamesIds[i] = m_attributes[i].m_nameId;
        qCDebug(Shaders) << "Active Attribute " << attributesDescription[i].m_name;
    }
}

void RHIShader::initializeUniformBlocks(const QVector<ShaderUniformBlock> &uniformBlockDescription)
{
    m_uniformBlocks = uniformBlockDescription;
    m_uniformBlockNames.resize(uniformBlockDescription.size());
    m_uniformBlockNamesIds.resize(uniformBlockDescription.size());
    for (int i = 0, m = uniformBlockDescription.size(); i < m; ++i) {
        m_uniformBlockNames[i] = m_uniformBlocks[i].m_name;
        m_uniformBlockNamesIds[i] = StringToInt::lookupId(m_uniformBlockNames[i]);
        m_uniformBlocks[i].m_nameId = m_uniformBlockNamesIds[i];
        qCDebug(Shaders) << "Initializing Uniform Block {" << m_uniformBlockNames[i] << "}";

        // Find all active uniforms for the shader block
        QVector<ShaderUniform>::const_iterator uniformsIt = m_uniforms.cbegin();
        const QVector<ShaderUniform>::const_iterator uniformsEnd = m_uniforms.cend();

        QVector<QString>::const_iterator uniformNamesIt = m_uniformsNames.cbegin();
        const QVector<QString>::const_iterator uniformNamesEnd = m_attributesNames.cend();

        QHash<QString, ShaderUniform> activeUniformsInBlock;

        while (uniformsIt != uniformsEnd && uniformNamesIt != uniformNamesEnd) {
            if (uniformsIt->m_blockIndex == uniformBlockDescription[i].m_index) {
                QString uniformName = *uniformNamesIt;
                if (!m_uniformBlockNames[i].isEmpty() && !uniformName.startsWith(m_uniformBlockNames[i]))
                    uniformName = m_uniformBlockNames[i] + QLatin1Char('.') + *uniformNamesIt;
                activeUniformsInBlock.insert(uniformName, *uniformsIt);
                qCDebug(Shaders) << "Active Uniform Block " << uniformName << " in block " << m_uniformBlockNames[i] << " at index " << uniformsIt->m_blockIndex;
            }
            ++uniformsIt;
            ++uniformNamesIt;
        }
        m_uniformBlockIndexToShaderUniforms.insert(uniformBlockDescription[i].m_index, activeUniformsInBlock);
    }
}

void RHIShader::initializeShaderStorageBlocks(const QVector<ShaderStorageBlock> &shaderStorageBlockDescription)
{
    m_shaderStorageBlocks = shaderStorageBlockDescription;
    m_shaderStorageBlockNames.resize(shaderStorageBlockDescription.size());
    m_shaderStorageBlockNamesIds.resize(shaderStorageBlockDescription.size());

    for (int i = 0, m = shaderStorageBlockDescription.size(); i < m; ++i) {
        m_shaderStorageBlockNames[i] = m_shaderStorageBlocks[i].m_name;
        m_shaderStorageBlockNamesIds[i] = StringToInt::lookupId(m_shaderStorageBlockNames[i]);
        m_shaderStorageBlocks[i].m_nameId =m_shaderStorageBlockNamesIds[i];
        qCDebug(Shaders) << "Initializing Shader Storage Block {" << m_shaderStorageBlockNames[i] << "}";
    }
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
