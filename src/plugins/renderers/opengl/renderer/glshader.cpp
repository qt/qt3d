// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "glshader_p.h"
#include <QMutexLocker>
#include <Qt3DRender/private/stringtoint_p.h>
#include <graphicscontext_p.h>
#include <logging_p.h>
#include <gllights_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

namespace {

std::vector<int> getLightUniformNameIds()
{
    std::vector<int> names;
    names.reserve(MAX_LIGHTS * 18 + 1);

    names.push_back(GLLights::LIGHT_COUNT_NAME_ID);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        names.push_back(GLLights::LIGHT_TYPE_NAMES[i]);
        names.push_back(GLLights::LIGHT_COLOR_NAMES[i]);
        names.push_back(GLLights::LIGHT_POSITION_NAMES[i]);
        names.push_back(GLLights::LIGHT_INTENSITY_NAMES[i]);
        names.push_back(GLLights::LIGHT_DIRECTION_NAMES[i]);
        names.push_back(GLLights::LIGHT_LINEAR_ATTENUATION_NAMES[i]);
        names.push_back(GLLights::LIGHT_QUADRATIC_ATTENUATION_NAMES[i]);
        names.push_back(GLLights::LIGHT_CONSTANT_ATTENUATION_NAMES[i]);
        names.push_back(GLLights::LIGHT_CUT_OFF_ANGLE_NAMES[i]);
        names.push_back(GLLights::LIGHT_TYPE_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_COLOR_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_POSITION_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_INTENSITY_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_DIRECTION_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_LINEAR_ATTENUATION_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_QUADRATIC_ATTENUATION_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_CONSTANT_ATTENUATION_UNROLL_NAMES[i]);
        names.push_back(GLLights::LIGHT_CUT_OFF_ANGLE_UNROLL_NAMES[i]);
    }

    return names;
}

template<typename Vector>
bool fastContains(const Vector &v, int value)
{
    return std::binary_search(v.cbegin(), v.cend(), value);
}

}

GLShader::GLShader()
    : m_isLoaded(false)
    , m_graphicsContext(nullptr)
    , m_parameterPackSize(0)
    , m_hasActiveVariables(false)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

GLShader::~GLShader()
{
    if (m_contextConnection)
        QObject::disconnect(m_contextConnection);
}

void GLShader::setGraphicsContext(GraphicsContext *context)
{
    QMutexLocker lock(&m_mutex);
    m_graphicsContext = context;
    if (m_graphicsContext) {
        m_contextConnection = QObject::connect(m_graphicsContext->openGLContext(),
                                               &QOpenGLContext::aboutToBeDestroyed,
                                               m_graphicsContext->openGLContext(),
                                               [this] { setGraphicsContext(nullptr); });
    }
}

GraphicsContext *GLShader::graphicsContext()
{
    QMutexLocker lock(&m_mutex);
    return m_graphicsContext;
}


const std::vector<QString> &GLShader::uniformsNames() const
{
    return m_uniformsNames;
}

const std::vector<QString> &GLShader::attributesNames() const
{
    return m_attributesNames;
}

const std::vector<QString> &GLShader::uniformBlockNames() const
{
    return m_uniformBlockNames;
}

const std::vector<QString> &GLShader::storageBlockNames() const
{
    return m_shaderStorageBlockNames;
}

const std::vector<QByteArray> &GLShader::shaderCode() const
{
    return m_shaderCode;
}

QHash<QString, ShaderUniform> GLShader::activeUniformsForUniformBlock(int blockIndex) const
{
    return m_uniformBlockIndexToShaderUniforms.value(blockIndex);
}

ShaderUniformBlock GLShader::uniformBlockForBlockIndex(int blockIndex) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_index == blockIndex) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock GLShader::uniformBlockForBlockNameId(int blockNameId) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_nameId == blockNameId) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock GLShader::uniformBlockForBlockName(const QString &blockName) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_name == blockName) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockIndex(int blockIndex) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_index == blockIndex)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockNameId(int blockNameId) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_nameId == blockNameId)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockName(const QString &blockName) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_name == blockName)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

GLShader::ParameterKind GLShader::categorizeVariable(int nameId) const noexcept
{
    if (fastContains(m_uniformsNamesIds, nameId))
        return ParameterKind::Uniform;
    if (fastContains(m_uniformBlockNamesIds, nameId))
        return ParameterKind::UBO;
    if (fastContains(m_shaderStorageBlockNamesIds, nameId))
        return ParameterKind::SSBO;
    return ParameterKind::Struct;
}

bool GLShader::hasUniform(int nameId) const noexcept
{
    return Qt3DCore::contains(m_uniformsNamesIds, nameId);
}

void GLShader::prepareUniforms(ShaderParameterPack &pack)
{
    const PackUniformHash &values = pack.uniforms();

    auto it = values.keys.cbegin();
    const auto end = values.keys.cend();

    const int shaderUniformsCount = int(m_uniforms.size());
    const auto uIt = m_uniforms.cbegin();

    while (it != end) {
        // Find if there's a uniform with the same name id

        int i = 0;
        const int targetNameId = *it;
        while (i < shaderUniformsCount && (uIt + i)->m_nameId < targetNameId)
            ++i;

        if (i < shaderUniformsCount && (uIt + i)->m_nameId == targetNameId)
            pack.setSubmissionUniformIndex(i);

        ++it;
    }
}

void GLShader::setFragOutputs(const QHash<QString, int> &fragOutputs)
{
    {
        QMutexLocker lock(&m_mutex);
        m_fragOutputs = fragOutputs;
    }
}

const QHash<QString, int> GLShader::fragOutputs() const
{
    QMutexLocker lock(&m_mutex);
    return m_fragOutputs;
}

void GLShader::initializeUniforms(const std::vector<ShaderUniform> &uniformsDescription)
{
    m_uniforms = uniformsDescription;
    m_uniformsNames.resize(uniformsDescription.size());
    m_uniformsNamesIds.reserve(uniformsDescription.size());
    m_standardUniformNamesIds.reserve(5);
    m_lightUniformsNamesIds.reserve(MAX_LIGHTS * 8 + 1);
    QHash<QString, ShaderUniform> activeUniformsInDefaultBlock;

    static const std::vector<int> standardUniformNameIds = {
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

    static const std::vector<int> lightUniformNameIds = getLightUniformNameIds();

    for (size_t i = 0, m = uniformsDescription.size(); i < m; i++) {
        m_uniformsNames[i] = m_uniforms[i].m_name;
        const int nameId = StringToInt::lookupId(m_uniformsNames[i]);
        m_uniforms[i].m_nameId = nameId;

        // Is the uniform a Qt3D "Standard" uniform, a light uniform or a user defined one?
        if (Qt3DCore::contains(standardUniformNameIds, nameId))
            m_standardUniformNamesIds.push_back(nameId);
        else if (Qt3DCore::contains(lightUniformNameIds, nameId))
            m_lightUniformsNamesIds.push_back(nameId);
        else
            m_uniformsNamesIds.push_back(nameId);

        if (uniformsDescription[i].m_blockIndex == -1) { // Uniform is in default block
            qCDebug(Shaders) << "Active Uniform in Default Block " << uniformsDescription[i].m_name << uniformsDescription[i].m_blockIndex;
            activeUniformsInDefaultBlock.insert(uniformsDescription[i].m_name, uniformsDescription[i]);
        }
    }
    m_uniformBlockIndexToShaderUniforms.insert(-1, activeUniformsInDefaultBlock);

    m_parameterPackSize += int(m_standardUniformNamesIds.size()) + int(m_lightUniformsNamesIds.size()) + int(m_uniformsNamesIds.size());
    m_hasActiveVariables |= (m_parameterPackSize > 0);

    // Sort by ascending order to make contains check faster
    std::sort(m_uniformsNamesIds.begin(), m_uniformsNamesIds.end());
    std::sort(m_lightUniformsNamesIds.begin(), m_lightUniformsNamesIds.end());
    std::sort(m_standardUniformNamesIds.begin(), m_standardUniformNamesIds.end());
    std::sort(m_uniforms.begin(), m_uniforms.end(),
              [] (const ShaderUniform &a, const ShaderUniform &b) {
        return a.m_nameId < b.m_nameId;
    });
}

void GLShader::initializeAttributes(const std::vector<ShaderAttribute> &attributesDescription)
{
    m_attributes = attributesDescription;
    m_attributesNames.resize(attributesDescription.size());
    m_attributeNamesIds.resize(attributesDescription.size());
    for (int i = 0, m = int(attributesDescription.size()); i < m; i++) {
        m_attributesNames[i] = attributesDescription[i].m_name;
        m_attributes[i].m_nameId = StringToInt::lookupId(m_attributesNames[i]);
        m_attributeNamesIds[i] = m_attributes[i].m_nameId;
        qCDebug(Shaders) << "Active Attribute " << attributesDescription[i].m_name;
    }
    m_hasActiveVariables |= (m_attributeNamesIds.size() > 0);
}

void GLShader::initializeUniformBlocks(const std::vector<ShaderUniformBlock> &uniformBlockDescription)
{
    m_uniformBlocks = uniformBlockDescription;
    m_uniformBlockNames.resize(uniformBlockDescription.size());
    m_uniformBlockNamesIds.resize(uniformBlockDescription.size());
    for (int i = 0, m = int(uniformBlockDescription.size()); i < m; ++i) {
        m_uniformBlockNames[i] = m_uniformBlocks[i].m_name;
        m_uniformBlockNamesIds[i] = StringToInt::lookupId(m_uniformBlockNames[i]);
        m_uniformBlocks[i].m_nameId = m_uniformBlockNamesIds[i];
        qCDebug(Shaders) << "Initializing Uniform Block {" << m_uniformBlockNames[i] << "}";

        // Find all active uniforms for the shader block
        std::vector<ShaderUniform>::const_iterator uniformsIt = m_uniforms.cbegin();
        const std::vector<ShaderUniform>::const_iterator uniformsEnd = m_uniforms.cend();

        std::vector<QString>::const_iterator uniformNamesIt = m_uniformsNames.cbegin();
        const std::vector<QString>::const_iterator uniformNamesEnd = m_uniformsNames.cend();

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

    m_parameterPackSize += int(m_uniformsNamesIds.size());
    m_hasActiveVariables |= (m_parameterPackSize > 0);

    // Sort by ascending order to make contains check faster
    std::sort(m_uniformBlockNamesIds.begin(), m_uniformBlockNamesIds.end());
}

void GLShader::initializeShaderStorageBlocks(const std::vector<ShaderStorageBlock> &shaderStorageBlockDescription)
{
    m_shaderStorageBlocks = shaderStorageBlockDescription;
    m_shaderStorageBlockNames.resize(shaderStorageBlockDescription.size());
    m_shaderStorageBlockNamesIds.resize(shaderStorageBlockDescription.size());

    for (int i = 0, m = int(shaderStorageBlockDescription.size()); i < m; ++i) {
        m_shaderStorageBlockNames[i] = m_shaderStorageBlocks[i].m_name;
        m_shaderStorageBlockNamesIds[i] = StringToInt::lookupId(m_shaderStorageBlockNames[i]);
        m_shaderStorageBlocks[i].m_nameId =m_shaderStorageBlockNamesIds[i];
        qCDebug(Shaders) << "Initializing Shader Storage Block {" << m_shaderStorageBlockNames[i] << "}";
    }

    m_parameterPackSize += int(m_shaderStorageBlockNamesIds.size());
    m_hasActiveVariables |= (m_parameterPackSize > 0);

    // Sort by ascending order to make contains check faster
    std::sort(m_shaderStorageBlockNamesIds.begin(), m_shaderStorageBlockNamesIds.end());
}

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE
