// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "rhishader_p.h"
#include <QMutexLocker>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <submissioncontext_p.h>
#include <logging_p.h>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RHIShader::RHIShader() : m_isLoaded(false)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

const std::vector<QString> &RHIShader::uniformsNames() const
{
    return m_uniformsNames;
}

const std::vector<QString> &RHIShader::attributesNames() const
{
    return m_attributesNames;
}

const std::vector<QString> &RHIShader::uniformBlockNames() const
{
    return m_uniformBlockNames;
}

const std::vector<QString> &RHIShader::storageBlockNames() const
{
    return m_shaderStorageBlockNames;
}

const std::vector<QString> &RHIShader::samplerNames() const
{
    return m_samplerNames;
}

const std::vector<QString> &RHIShader::imagesNames() const
{
    return m_imageNames;
}

const std::vector<QByteArray> &RHIShader::shaderCode() const
{
    return m_shaderCode;
}

namespace {

static constexpr int rhiTypeSize(QShaderDescription::VariableType type)
{
    switch (type) {
    case QShaderDescription::Unknown:
        return 0;

    case QShaderDescription::Float:
        return 1;
    case QShaderDescription::Vec2:
        return 2;
    case QShaderDescription::Vec3:
        return 3;
    case QShaderDescription::Vec4:
        return 4;
    case QShaderDescription::Mat2:
        return 2 * 2;
    case QShaderDescription::Mat2x3:
        return 2 * 3;
    case QShaderDescription::Mat2x4:
        return 2 * 4;
    case QShaderDescription::Mat3:
        return 3 * 3;
    case QShaderDescription::Mat3x2:
        return 3 * 2;
    case QShaderDescription::Mat3x4:
        return 3 * 4;
    case QShaderDescription::Mat4:
        return 4 * 4;
    case QShaderDescription::Mat4x2:
        return 4 * 2;
    case QShaderDescription::Mat4x3:
        return 4 * 3;

    case QShaderDescription::Int:
        return 1;
    case QShaderDescription::Int2:
        return 2;
    case QShaderDescription::Int3:
        return 3;
    case QShaderDescription::Int4:
        return 4;

    case QShaderDescription::Uint:
        return 1;
    case QShaderDescription::Uint2:
        return 2;
    case QShaderDescription::Uint3:
        return 3;
    case QShaderDescription::Uint4:
        return 4;

    case QShaderDescription::Bool:
        return 1;
    case QShaderDescription::Bool2:
        return 2;
    case QShaderDescription::Bool3:
        return 3;
    case QShaderDescription::Bool4:
        return 4;

    case QShaderDescription::Double:
        return 1;
    case QShaderDescription::Double2:
        return 2;
    case QShaderDescription::Double3:
        return 3;
    case QShaderDescription::Double4:
        return 4;
    case QShaderDescription::DMat2:
        return 2 * 2;
    case QShaderDescription::DMat2x3:
        return 2 * 3;
    case QShaderDescription::DMat2x4:
        return 2 * 4;
    case QShaderDescription::DMat3:
        return 3 * 3;
    case QShaderDescription::DMat3x2:
        return 3 * 2;
    case QShaderDescription::DMat3x4:
        return 3 * 4;
    case QShaderDescription::DMat4:
        return 4 * 4;
    case QShaderDescription::DMat4x2:
        return 4 * 2;
    case QShaderDescription::DMat4x3:
        return 4 * 3;

    case QShaderDescription::Sampler1D:
        return 0;
    case QShaderDescription::Sampler2D:
        return 0;
    case QShaderDescription::Sampler2DMS:
        return 0;
    case QShaderDescription::Sampler3D:
        return 0;
    case QShaderDescription::SamplerCube:
        return 0;
    case QShaderDescription::Sampler1DArray:
        return 0;
    case QShaderDescription::Sampler2DArray:
        return 0;
    case QShaderDescription::Sampler2DMSArray:
        return 0;
    case QShaderDescription::Sampler3DArray:
        return 0;
    case QShaderDescription::SamplerCubeArray:
        return 0;
    case QShaderDescription::SamplerRect:
        return 0;
    case QShaderDescription::SamplerBuffer:
        return 0;

    case QShaderDescription::Image1D:
        return 0;
    case QShaderDescription::Image2D:
        return 0;
    case QShaderDescription::Image2DMS:
        return 0;
    case QShaderDescription::Image3D:
        return 0;
    case QShaderDescription::ImageCube:
        return 0;
    case QShaderDescription::Image1DArray:
        return 0;
    case QShaderDescription::Image2DArray:
        return 0;
    case QShaderDescription::Image2DMSArray:
        return 0;
    case QShaderDescription::Image3DArray:
        return 0;
    case QShaderDescription::ImageCubeArray:
        return 0;
    case QShaderDescription::ImageRect:
        return 0;
    case QShaderDescription::ImageBuffer:
        return 0;

    case QShaderDescription::Struct:
        return 0;
    default:
        return 0;
    }
}

template<typename T, typename Pred>
std::vector<T> stableRemoveDuplicates(std::vector<T> in, Pred predicate)
{
    std::vector<T> out;
    for (const auto &element : in) {
        if (std::none_of(out.begin(), out.end(),
                         [&](T &other) { return predicate(element, other); }))
            out.push_back(element);
    }
    return out;
}

// Utility function to enumerate an array of dimensions
// Given dims == [0, 3, 2] and maxs == [4, 4, 4]
// changes dims into [0, 3, 3]
// Given dims == [0, 3, 3] and maxs == [4, 4, 4]
// changes dims into [1, 0, 0]
bool incrementArray(QVarLengthArray<int> &dims, const QList<int> &maxs)
{
    const int n = dims.size();
    int i = n;
    for (; i-- > 0;) {
        if (dims[i] == maxs[i] - 1) {
            if (i == 0) {
                // we're done
                return false;
            }
            continue;

        } else {
            dims[i]++;
            for (int j = i + 1; j < n; j++) {
                dims[j] = 0;
            }
            return true;
        }
    }
    return false;
}

// Call a function with a string such as [0][3][2]
// for all valable array values, given an array of dimension sizes.
// Dimensions must all be >= 1
template<typename F>
void forEachArrayAccessor(const QList<int> &maxs, F f)
{
    if (std::any_of(maxs.begin(), maxs.end(), [](int v) { return v <= 0; }))
        return;

    QVarLengthArray<int> dims;
    dims.resize(maxs.size());

    // QVarLengthArray does not initialize ints
    std::fill(dims.begin(), dims.end(), 0);

    QString str;

    do {
        str.resize(0);
        for (int k : dims) {
            str += QStringLiteral("[%1]").arg(k);
        }
        f(str);
    } while (incrementArray(dims, maxs));
}
}

void RHIShader::recordAllUniforms(UBO_Member &uboMember,
                                  QString parentName)
{
    const QShaderDescription::BlockVariable &member = uboMember.blockVariable;
    const bool isStruct = !member.structMembers.empty();
    const bool isArray = !member.arrayDims.empty();

    // "foo.bar"
    const QString fullMemberName = parentName + member.name;
    m_unqualifiedUniformNames << fullMemberName;

    if (isStruct && !isArray) {
        m_structNames.push_back(fullMemberName);
        m_structNamesIds.push_back(StringToInt::lookupId(fullMemberName));

        for (const QShaderDescription::BlockVariable& bv : member.structMembers) {
            UBO_Member innerMember {StringToInt::lookupId(fullMemberName), bv, {}};
            // recordAllUniforms("baz", "foo.bar.")
            const QString structMemberNamePrefix = fullMemberName + QLatin1Char('.');
            recordAllUniforms(innerMember, structMemberNamePrefix);
            uboMember.structMembers.push_back(innerMember);
        }
    } else if (!isStruct && isArray) {
        // We iterate through all the [l][n][m] by building [0][0][0] and incrementing
        forEachArrayAccessor(member.arrayDims, [&](const QString &str) {
            // "foo.bar[1][2]"
            const QString unqualifiedMemberName = (fullMemberName + str);
            m_unqualifiedUniformNames << unqualifiedMemberName;

            // Record as an individual uniform
            m_uniformsNames.push_back(unqualifiedMemberName);
            const int nameId = StringToInt::lookupId(unqualifiedMemberName);
            m_uniformsNamesIds.push_back(nameId);

            // Question : does it make sense to also record foo[0], foo[0][0], etc...
            // if there are e.g. 3 dimensions ?
        });
    }
    else if (isStruct && isArray) {
        // Record the struct names
        forEachArrayAccessor(member.arrayDims, [&] (const QString& str) {
            m_structNames.push_back(fullMemberName + str);
            m_structNamesIds.push_back(StringToInt::lookupId(m_structNames.back()));
        });

        // Record the array times the struct members => entry[i].struct_member
        forEachArrayAccessor(member.arrayDims, [&] (const QString& str) {
            UBO_Member arrayMember {StringToInt::lookupId(fullMemberName + str), {}, {}};
            // Record all struct member into the array member[i]
            for (const QShaderDescription::BlockVariable& bv : member.structMembers) {
                //recordAllUniforms("baz", "foo.bar[1][2].")
                const QString structMemberNamePrefix = fullMemberName + str + QLatin1Char('.');
                UBO_Member innerMember {StringToInt::lookupId(structMemberNamePrefix), bv, {}};
                recordAllUniforms(innerMember, structMemberNamePrefix);
                arrayMember.structMembers.push_back(innerMember);
            }
            // When dealing with an array of structs, we treat structMembers as arrayMembers
            uboMember.structMembers.push_back(arrayMember);
        });
    } else {
        // Final member (not array or struct)
        // Replace nameId with final nameId name
        uboMember.nameId = StringToInt::lookupId(fullMemberName);

        // Record as an individual uniform
        m_uniformsNames.push_back(fullMemberName);
        const int nameId = StringToInt::lookupId(fullMemberName);
        m_uniformsNamesIds.push_back(nameId);
    }
}

namespace {
// Function to check whether the array conforms to the regex
// "_[0-9]+" - except QRegularExpression does not support QByteArray
bool isGeneratedUBOName(const QByteArray& arr)
{
    if (arr.size() < 2)
        return false;
    if (!arr.startsWith('_'))
        return false;
    for (qsizetype i = 1, N = arr.size(); i < N; i++) {
        if (arr[i] < '0' || arr[i] > '9')
            return false;
    }
    return true;
}
}

void RHIShader::introspect()
{
    std::vector<QShaderDescription::UniformBlock> rhiUBO;
    std::vector<QShaderDescription::StorageBlock> rhiSSBO;

    std::vector<ShaderUniformBlock> uniformBlocks;
    std::vector<ShaderStorageBlock> storageBlocks;
    std::vector<ShaderAttribute> attributes;
    std::vector<ShaderAttribute> samplers;
    std::vector<ShaderAttribute> images;

    if (m_stages[QShader::ComputeStage].isValid()) {
        const QShaderDescription &comp = m_stages[QShader::ComputeStage].description();

        Qt3DCore::append(rhiUBO, comp.uniformBlocks());
        Qt3DCore::append(rhiSSBO, comp.storageBlocks());
    }
    else
    {
        // Introspect shader vertex input
        if (m_stages[QShader::VertexStage].isValid()) {
            const QShaderDescription &vtx = m_stages[QShader::VertexStage].description();

            for (const QShaderDescription::InOutVariable &input : vtx.inputVariables()) {
                attributes.push_back(ShaderAttribute { input.name, StringToInt::lookupId(input.name),
                                                       input.type, rhiTypeSize(input.type),
                                                       input.location });
            }

            Qt3DCore::append(rhiUBO, vtx.uniformBlocks());
            Qt3DCore::append(rhiSSBO, vtx.storageBlocks());
        }

        // Introspect shader uniforms
        if (m_stages[QShader::FragmentStage].isValid()) {
            const QShaderDescription &frag = m_stages[QShader::FragmentStage].description();
            for (const QShaderDescription::InOutVariable &sampler : frag.combinedImageSamplers()) {
                samplers.push_back(ShaderAttribute { sampler.name, StringToInt::lookupId(sampler.name),
                                                     sampler.type, rhiTypeSize(sampler.type),
                                                     sampler.binding });
            }
            for (const QShaderDescription::InOutVariable &image : frag.storageImages()) {
                images.push_back(ShaderAttribute { image.name, StringToInt::lookupId(image.name),
                                                   image.type, rhiTypeSize(image.type),
                                                   image.binding });
            }

            Qt3DCore::append(rhiUBO, frag.uniformBlocks());
            Qt3DCore::append(rhiSSBO, frag.storageBlocks());
        }
    }

    rhiUBO = stableRemoveDuplicates(rhiUBO,
                                    [](const QShaderDescription::UniformBlock &lhs,
                                       const QShaderDescription::UniformBlock &rhs) {
                                        return lhs.blockName == rhs.blockName;
                                    });
    rhiSSBO = stableRemoveDuplicates(rhiSSBO,
                                     [](const QShaderDescription::StorageBlock &lhs,
                                        const QShaderDescription::StorageBlock &rhs) {
                                         return lhs.blockName == rhs.blockName;
                                     });

    for (const QShaderDescription::UniformBlock &ubo : rhiUBO) {
        uniformBlocks.push_back(ShaderUniformBlock { ubo.blockName,
                                                     StringToInt::lookupId(ubo.blockName), -1,
                                                     ubo.binding, int(ubo.members.size()), ubo.size });
        const bool addUnqualifiedUniforms = isGeneratedUBOName(ubo.structName);

        // Parse Uniform Block members so that we can later on map a Parameter name to an actual
        m_uniformsNames.reserve(m_uniformsNames.size() + ubo.members.size());
        m_uniformsNamesIds.reserve(m_uniformsNames.size());

        std::vector<UBO_Member> uboMembers;
        uboMembers.reserve(ubo.members.size());

        for (const QShaderDescription::BlockVariable &member : std::as_const(ubo.members)) {
            m_uniformsNames.push_back(member.name);
            const int nameId = StringToInt::lookupId(member.name);
            m_uniformsNamesIds.push_back(nameId);

            UBO_Member uboMember {nameId, member, {}};
            if (addUnqualifiedUniforms)
                recordAllUniforms(uboMember, QStringLiteral(""));
            uboMembers.push_back(uboMember);
        }

        m_uboBlocks.push_back(UBO_Block{ uniformBlocks.back(), uboMembers });
    }

    for (const QShaderDescription::StorageBlock &ssbo : rhiSSBO) {
        storageBlocks.push_back(ShaderStorageBlock { ssbo.blockName, -1, -1, ssbo.binding, 0, 0 });
    }

    initializeAttributes(attributes);
    initializeUniformBlocks(uniformBlocks);
    initializeShaderStorageBlocks(storageBlocks);
    initializeSamplers(samplers);
    initializeImages(images);

    qCDebug(Shaders) << "Block Names" << m_uniformBlockNames;
    qCDebug(Shaders) << "Uniform Names" << m_uniformsNames;
    qCDebug(Shaders) << "SSBO Names" << m_shaderStorageBlockNames;
    qCDebug(Shaders) << "Attribute Names" << m_attributesNames;

}

QHash<QString, ShaderUniform> RHIShader::activeUniformsForUniformBlock(int blockIndex) const
{
    return m_uniformBlockIndexToShaderUniforms.value(blockIndex);
}

ShaderUniformBlock RHIShader::uniformBlockForBlockIndex(int blockIndex) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_index == blockIndex) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock RHIShader::uniformBlockForBlockNameId(int blockNameId) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_nameId == blockNameId) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock RHIShader::uniformBlockForBlockName(const QString &blockName) const noexcept
{
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
        if (m_uniformBlocks[i].m_name == blockName) {
            return m_uniformBlocks[i];
        }
    }
    return ShaderUniformBlock();
}

ShaderUniformBlock RHIShader::uniformBlockForInstanceName(const QString &instanceName) const noexcept
{
    return uniformBlockForInstanceNameId(StringToInt::lookupId(instanceName));
}

ShaderUniformBlock RHIShader::uniformBlockForInstanceNameId(int instanceNameId) const noexcept
{
    for (size_t i = 0, m = m_uboBlocks.size(); i < m; ++i) {
        const UBO_Block &b = m_uboBlocks[i];
        for (const UBO_Member &member : b.members) {
            if (member.nameId == instanceNameId)
                return b.block;
        }
    }
    return ShaderUniformBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockIndex(int blockIndex) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_index == blockIndex)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockNameId(int blockNameId) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_nameId == blockNameId)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

ShaderStorageBlock RHIShader::storageBlockForBlockName(const QString &blockName) const noexcept
{
    for (size_t i = 0, m = m_shaderStorageBlockNames.size(); i < m; ++i) {
        if (m_shaderStorageBlocks[i].m_name == blockName)
            return m_shaderStorageBlocks[i];
    }
    return ShaderStorageBlock();
}

RHIShader::ParameterKind RHIShader::categorizeVariable(int nameId) const noexcept
{
    if (Qt3DCore::contains(m_uniformBlockNamesIds, nameId))
        return ParameterKind::UBO;
    else if (Qt3DCore::contains(m_shaderStorageBlockNamesIds, nameId))
        return ParameterKind::SSBO;
    else if (Qt3DCore::contains(m_structNamesIds, nameId))
        return ParameterKind::Struct;
    return ParameterKind::Uniform;
}

bool RHIShader::hasUniform(int nameId) const noexcept
{
    return Qt3DCore::contains(m_uniformsNamesIds, nameId);
}

bool RHIShader::hasActiveVariables() const noexcept
{
    return !m_attributeNamesIds.empty() || !m_uniformsNamesIds.empty()
            || !m_uniformBlockNamesIds.empty() || !m_shaderStorageBlockNamesIds.empty();
}

void RHIShader::setShaderCode(std::vector<QByteArray> shaderCode)
{
    m_shaderCode = std::move(shaderCode);
}

void RHIShader::setFragOutputs(QHash<QString, int> fragOutputs)
{
    {
        QMutexLocker lock(&m_mutex);
        m_fragOutputs = std::move(fragOutputs);
    }
    //    updateDNA();
}

const QHash<QString, int> &RHIShader::fragOutputs() const
{
    QMutexLocker lock(&m_mutex);
    return m_fragOutputs;
}

void RHIShader::initializeAttributes(std::vector<ShaderAttribute> attributesDescription)
{
    m_attributes = std::move(attributesDescription);
    m_attributesNames.resize(m_attributes.size());
    m_attributeNamesIds.resize(m_attributes.size());
    for (size_t i = 0, m = m_attributes.size(); i < m; i++) {
        m_attributesNames[i] = m_attributes[i].m_name;
        m_attributes[i].m_nameId = StringToInt::lookupId(m_attributesNames[i]);
        m_attributeNamesIds[i] = m_attributes[i].m_nameId;
        qCDebug(Shaders) << "Active Attribute " << m_attributes[i].m_name;
    }
}

void RHIShader::initializeSamplers(std::vector<ShaderAttribute> samplersDescription)
{
    m_samplers = std::move(samplersDescription);
    m_samplerNames.resize(m_samplers.size());
    m_samplerIds.resize(m_samplers.size());
    for (size_t i = 0, m = m_samplers.size(); i < m; i++) {
        m_samplerNames[i] = m_samplers[i].m_name;
        m_samplers[i].m_nameId = StringToInt::lookupId(m_samplerNames[i]);
        m_samplerIds[i] = m_samplers[i].m_nameId;
        qCDebug(Shaders) << "Active sampler " << m_samplers[i].m_name;
    }
}

void RHIShader::initializeImages(std::vector<ShaderAttribute> imagesDescription)
{
    m_images = std::move(imagesDescription);
    m_imageNames.resize(m_images.size());
    m_imageIds.resize(m_images.size());
    for (size_t i = 0, m = m_images.size(); i < m; i++) {
        m_imageNames[i] = m_images[i].m_name;
        m_images[i].m_nameId = StringToInt::lookupId(m_imageNames[i]);
        m_imageIds[i] = m_images[i].m_nameId;
        qCDebug(Shaders) << "Active image " << m_images[i].m_name;
    }
}

void RHIShader::initializeUniformBlocks(std::vector<ShaderUniformBlock> uniformBlockDescription)
{
    m_uniformBlocks = std::move(uniformBlockDescription);
    m_uniformBlockNames.resize(m_uniformBlocks.size());
    m_uniformBlockNamesIds.resize(m_uniformBlocks.size());
    for (size_t i = 0, m = m_uniformBlocks.size(); i < m; ++i) {
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
            if (uniformsIt->m_blockIndex == m_uniformBlocks[i].m_index) {
                QString uniformName = *uniformNamesIt;
                if (!m_uniformBlockNames[i].isEmpty()
                    && !uniformName.startsWith(m_uniformBlockNames[i]))
                    uniformName = m_uniformBlockNames[i] + QLatin1Char('.') + *uniformNamesIt;
                activeUniformsInBlock.insert(uniformName, *uniformsIt);
                qCDebug(Shaders) << "Active Uniform Block " << uniformName << " in block "
                                 << m_uniformBlockNames[i] << " at index "
                                 << uniformsIt->m_blockIndex;
            }
            ++uniformsIt;
            ++uniformNamesIt;
        }
        m_uniformBlockIndexToShaderUniforms.insert(m_uniformBlocks[i].m_index,
                                                   activeUniformsInBlock);
    }
}

void RHIShader::initializeShaderStorageBlocks(
        std::vector<ShaderStorageBlock> shaderStorageBlockDescription)
{
    m_shaderStorageBlocks = std::move(shaderStorageBlockDescription);
    m_shaderStorageBlockNames.resize(m_shaderStorageBlocks.size());
    m_shaderStorageBlockNamesIds.resize(m_shaderStorageBlocks.size());

    for (size_t i = 0, m = m_shaderStorageBlocks.size(); i < m; ++i) {
        m_shaderStorageBlockNames[i] = m_shaderStorageBlocks[i].m_name;
        m_shaderStorageBlockNamesIds[i] = StringToInt::lookupId(m_shaderStorageBlockNames[i]);
        m_shaderStorageBlocks[i].m_nameId = m_shaderStorageBlockNamesIds[i];
        qCDebug(Shaders) << "Initializing Shader Storage Block {" << m_shaderStorageBlockNames[i]
                         << "}";
    }
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
