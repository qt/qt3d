/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "glshader_p.h"
#include <QMutexLocker>
#include <Qt3DRender/private/stringtoint_p.h>
#include <graphicscontext_p.h>
#include <logging_p.h>
#include <gllights_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

namespace {

QVector<int> getLightUniformNameIds()
{
    QVector<int> names;
    names.reserve(MAX_LIGHTS * 18 + 1);

    names << GLLights::LIGHT_COUNT_NAME_ID;
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        names << GLLights::LIGHT_TYPE_NAMES[i]
              << GLLights::LIGHT_COLOR_NAMES[i]
              << GLLights::LIGHT_POSITION_NAMES[i]
              << GLLights::LIGHT_INTENSITY_NAMES[i]
              << GLLights::LIGHT_DIRECTION_NAMES[i]
              << GLLights::LIGHT_LINEAR_ATTENUATION_NAMES[i]
              << GLLights::LIGHT_QUADRATIC_ATTENUATION_NAMES[i]
              << GLLights::LIGHT_CONSTANT_ATTENUATION_NAMES[i]
              << GLLights::LIGHT_CUT_OFF_ANGLE_NAMES[i]
              << GLLights::LIGHT_TYPE_UNROLL_NAMES[i]
              << GLLights::LIGHT_COLOR_UNROLL_NAMES[i]
              << GLLights::LIGHT_POSITION_UNROLL_NAMES[i]
              << GLLights::LIGHT_INTENSITY_UNROLL_NAMES[i]
              << GLLights::LIGHT_DIRECTION_UNROLL_NAMES[i]
              << GLLights::LIGHT_LINEAR_ATTENUATION_UNROLL_NAMES[i]
              << GLLights::LIGHT_QUADRATIC_ATTENUATION_UNROLL_NAMES[i]
              << GLLights::LIGHT_CONSTANT_ATTENUATION_UNROLL_NAMES[i]
              << GLLights::LIGHT_CUT_OFF_ANGLE_UNROLL_NAMES[i];
    }

    return names;
}

}

GLShader::GLShader()
    : m_isLoaded(false)
    , m_graphicsContext(nullptr)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

void GLShader::setGraphicsContext(GraphicsContext *context)
{
    QMutexLocker lock(&m_mutex);
    m_graphicsContext = context;
    if (m_graphicsContext) {
        m_contextConnection = QObject::connect(m_graphicsContext->openGLContext(),
                                               &QOpenGLContext::aboutToBeDestroyed,
                                               [this] { setGraphicsContext(nullptr); });
    }
}

GraphicsContext *GLShader::graphicsContext()
{
    QMutexLocker lock(&m_mutex);
    return m_graphicsContext;
}


QVector<QString> GLShader::uniformsNames() const
{
    return m_uniformsNames;
}

QVector<QString> GLShader::attributesNames() const
{
    return m_attributesNames;
}

QVector<QString> GLShader::uniformBlockNames() const
{
    return m_uniformBlockNames;
}

QVector<QString> GLShader::storageBlockNames() const
{
    return m_shaderStorageBlockNames;
}

QVector<QByteArray> GLShader::shaderCode() const
{
    return m_shaderCode;
}

QHash<QString, ShaderUniform> GLShader::activeUniformsForUniformBlock(int blockIndex) const
{
    return m_uniformBlockIndexToShaderUniforms.value(blockIndex);
}

ShaderUniformBlock GLShader::uniformBlockForBlockIndex(int blockIndex)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_index == blockIndex) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock GLShader::uniformBlockForBlockNameId(int blockNameId)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_nameId == blockNameId) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock GLShader::uniformBlockForBlockName(const QString &blockName)
{
    for (int i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_name == blockName) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockIndex(int blockIndex)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_index == blockIndex)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockNameId(int blockNameId)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_nameId == blockNameId)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock GLShader::storageBlockForBlockName(const QString &blockName)
{
    for (int i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_name == blockName)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

void GLShader::prepareUniforms(ShaderParameterPack &pack)
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

void GLShader::setFragOutputs(const QHash<QString, int> &fragOutputs)
{
    {
        QMutexLocker lock(&m_mutex);
        m_fragOutputs = fragOutputs;
    }
//    updateDNA();
}

const QHash<QString, int> GLShader::fragOutputs() const
{
    QMutexLocker lock(&m_mutex);
    return m_fragOutputs;
}

void GLShader::initializeUniforms(const QVector<ShaderUniform> &uniformsDescription)
{
    m_uniforms = uniformsDescription;
    m_uniformsNames.resize(uniformsDescription.size());
    m_uniformsNamesIds.reserve(uniformsDescription.size());
    m_standardUniformNamesIds.reserve(5);
    m_lightUniformsNamesIds.reserve(MAX_LIGHTS * 8 + 1);
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

    static const QVector<int> lightUniformNameIds = getLightUniformNameIds();

    for (int i = 0, m = uniformsDescription.size(); i < m; i++) {
        m_uniformsNames[i] = m_uniforms[i].m_name;
        const int nameId = StringToInt::lookupId(m_uniformsNames[i]);
        m_uniforms[i].m_nameId = nameId;

        // Is the uniform a Qt3D "Standard" uniform, a light uniform or a user defined one?
        if (standardUniformNameIds.contains(nameId))
            m_standardUniformNamesIds.push_back(nameId);
        else if (lightUniformNameIds.contains(nameId))
            m_lightUniformsNamesIds.push_back(nameId);
        else
            m_uniformsNamesIds.push_back(nameId);

        if (uniformsDescription[i].m_blockIndex == -1) { // Uniform is in default block
            qCDebug(Shaders) << "Active Uniform in Default Block " << uniformsDescription[i].m_name << uniformsDescription[i].m_blockIndex;
            activeUniformsInDefaultBlock.insert(uniformsDescription[i].m_name, uniformsDescription[i]);
        }
    }
    m_uniformBlockIndexToShaderUniforms.insert(-1, activeUniformsInDefaultBlock);
}

void GLShader::initializeAttributes(const QVector<ShaderAttribute> &attributesDescription)
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

void GLShader::initializeUniformBlocks(const QVector<ShaderUniformBlock> &uniformBlockDescription)
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

void GLShader::initializeShaderStorageBlocks(const QVector<ShaderStorageBlock> &shaderStorageBlockDescription)
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

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE
