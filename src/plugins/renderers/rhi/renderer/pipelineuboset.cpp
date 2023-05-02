// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "pipelineuboset_p.h"
#include <rendercommand_p.h>
#include <renderview_p.h>
#include <rhibuffer_p.h>
#include <shaderparameterpack_p.h>
#include <shadervariables_p.h>
#include <rhigraphicspipeline_p.h>
#include <rhiresourcemanagers_p.h>
#include <submissioncontext_p.h>
#include <rhi/qrhi.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

namespace {
// This is the minimum max UBO size requirements with GL
// therefore safe to assume that is also the baseline for other graphics APIs
constexpr size_t MaxUBOByteSize = 16384;
}

PipelineUBOSet::PipelineUBOSet()
{
}

PipelineUBOSet::~PipelineUBOSet()
{
    // Resource release is performed by RHIGraphicsPipeline::cleanup()
}

void PipelineUBOSet::clear()
{
    m_renderCommands.clear();
}

void PipelineUBOSet::addRenderCommand(const RenderCommand &cmd)
{
    m_renderCommands.push_back(&cmd);
}

void PipelineUBOSet::setResourceManager(RHIResourceManagers *manager)
{
    m_resourceManagers = manager;
}

void PipelineUBOSet::setNodeManagers(NodeManagers *managers)
{
    m_nodeManagers = managers;
}

void PipelineUBOSet::initializeLayout(SubmissionContext *ctx, RHIShader *shader)
{
    // We should only be called with a clean Pipeline
    Q_ASSERT(m_rvUBO.buffer.isNull());

    m_rvUBO.binding = 0;
    m_rvUBO.blockSize = sizeof(RenderViewUBO);

    m_commandsUBO.binding = 1;
    m_commandsUBO.blockSize = sizeof(CommandUBO);
    m_commandsUBO.alignedBlockSize = ctx->rhi()->ubufAligned((m_commandsUBO.blockSize));
    m_commandsUBO.alignment = size_t(ctx->rhi()->ubufAlignment());
    m_commandsUBO.commandsPerUBO = MaxUBOByteSize / m_commandsUBO.alignedBlockSize;

    // For UBO, we try to create a single large UBO that will contain frontend
    // Qt3D UBO data at various offsets
    const std::vector<ShaderUniformBlock> &uniformBlocks = shader->uniformBlocks();
    for (const ShaderUniformBlock &block : uniformBlocks) {
        if (block.m_binding > 1) { // Binding 0 and 1 are for RV and Command UBOs
            const size_t alignedBlockSize = size_t(ctx->rhi()->ubufAligned(block.m_size));
            m_materialsUBOs.push_back(
                        { block.m_binding,
                          block.m_size,
                          alignedBlockSize,
                          size_t(ctx->rhi()->ubufAlignment()),
                          MaxUBOByteSize / alignedBlockSize,
                          {} });
        }
    }

    // For SSBO, we have one SSBO per frontend Qt3D SSBO
    m_storageBlocks =  shader->storageBlocks();
}

void PipelineUBOSet::releaseResources()
{
    Q_ASSERT(m_resourceManagers);
    RHIBufferManager *bufferManager = m_resourceManagers->rhiBufferManager();

    bufferManager->release(m_rvUBO.buffer);

    for (const HRHIBuffer &hBuf : m_commandsUBO.buffers)
        bufferManager->release(hBuf);

    m_rvUBO = {};
    m_commandsUBO = {};

    if (!m_materialsUBOs.empty()) {
        for (const MultiUBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
            for (const HRHIBuffer &hBuf : ubo.buffers)
                bufferManager->release(hBuf);
        }
        m_materialsUBOs.clear();
    }
}

bool PipelineUBOSet::allocateUBOs(SubmissionContext *ctx)
{
    RHIBufferManager *bufferManager = m_resourceManagers->rhiBufferManager();
    // Note: RHIBuffer only releases/recreates when it needs more size than what it
    // can currently hold
    Q_ASSERT(m_resourceManagers);
    const bool dynamic = true;
    const size_t commandCount = std::max(m_renderCommands.size(), size_t(1));

    if (m_rvUBO.buffer.isNull())
        m_rvUBO.buffer = bufferManager->allocateResource();

    // RHIBuffer only reallocates if size is < than required
    m_rvUBO.buffer->allocate(QByteArray(m_rvUBO.blockSize, '\0'), dynamic);
    // Binding buffer ensure underlying RHI resource is created
    m_rvUBO.buffer->bind(ctx, RHIBuffer::UniformBuffer);

    auto allocateMultiUBOsForCommands = [&] (MultiUBOBufferWithBindingAndBlockSize &ubo) {
        // Round up
        const size_t uboCount = std::ceil(float(commandCount) / ubo.commandsPerUBO);

        if (ubo.buffers.size() < uboCount)
            ubo.buffers.resize(uboCount);

        for (HRHIBuffer &buf : ubo.buffers) {
            if (buf.isNull())
                buf = bufferManager->allocateResource();
            // We need to take into account any minimum alignment requirement for dynamic offsets
            buf->allocate(QByteArray(MaxUBOByteSize, '\0'), dynamic);
            buf->bind(ctx, RHIBuffer::UniformBuffer);
        }
    };

    // Commands UBOs
    allocateMultiUBOsForCommands(m_commandsUBO);

    // Material UBOs
    for (MultiUBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
        if (ubo.binding > 1) // Binding 0 and 1 are for RV and Command UBOs
            allocateMultiUBOsForCommands(ubo);
    }

    // SSBO are using RHIBuffer directly, nothing we need to handle ourselves

    return true;
}

size_t PipelineUBOSet::distanceToCommand(const RenderCommand &cmd) const
{
    const auto it = std::find(m_renderCommands.begin(), m_renderCommands.end(),
                              &cmd);
    if (Q_UNLIKELY(it == m_renderCommands.end())) {
        qCWarning(Backend) << "Command not found in UBOSet";
        return 0;
    }
    return std::distance(m_renderCommands.begin(), it);
}

std::vector<QRhiCommandBuffer::DynamicOffset> PipelineUBOSet::offsets(const RenderCommand &cmd) const
{
    std::vector<QRhiCommandBuffer::DynamicOffset> offsets;
    offsets.reserve(1 + m_materialsUBOs.size());

    // RenderCommand offset
    // binding, offset
    const size_t dToCmd = distanceToCommand(cmd);
    {
        // Compute offset relative to the select UBO in the subset
        const auto localOffset = m_commandsUBO.localOffsetInBufferForCommand(dToCmd);
        offsets.push_back({1, quint32(localOffset)});
    }

    for (const MultiUBOBufferWithBindingAndBlockSize &materialUBO : m_materialsUBOs) {
        // Compute offset relative to the select UBO in the subset
        const size_t localOffset = materialUBO.localOffsetInBufferForCommand(dToCmd);
        offsets.push_back({materialUBO.binding, quint32(localOffset)});
    }

    return offsets;
}

// Note: this function is currently the same as resourceBindings However, once
// https://codereview.qt-project.org/c/qt/qtbase/+/307472 lands we can get rid
// of passing the RenderCommand here as well as having to provide the actual
// Resources in that function which means that one will be about providing the
// Pipeline resourceLayout only and resourceBindings will be about providing
// the actual resources for a RenderCommand
std::vector<QRhiShaderResourceBinding> PipelineUBOSet::resourceLayout(const RHIShader *shader)
{
    const QRhiShaderResourceBinding::StageFlags stages = QRhiShaderResourceBinding::VertexStage|QRhiShaderResourceBinding::FragmentStage;
    std::vector<QRhiShaderResourceBinding> bindings = {
        QRhiShaderResourceBinding::uniformBuffer(0, stages, nullptr),
        QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(1, stages, nullptr, sizeof(CommandUBO))
    };

    // TO DO: Handle Parameters that directly define a UBO or SSBO
    //    const auto &blockToUBOs = parameterPack.uniformBuffers();
    //    const auto &blockToSSBOs = parameterPack.shaderStorageBuffers();

    // Create additional empty UBO Buffer for UBO with binding point > 1 (since
    // we assume 0 and 1 and for Qt3D standard values)
    for (const MultiUBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs)
        bindings.push_back(
                    QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(
                        ubo.binding, stages, nullptr, ubo.blockSize));

    // Samplers
    for (const ShaderAttribute &samplerAttribute : shader->samplers()) {
        bindings.push_back(QRhiShaderResourceBinding::sampledTexture(
                               samplerAttribute.m_location,
                               stages, nullptr, nullptr));
    }

    // SSBO
    for (const ShaderStorageBlock &b : m_storageBlocks) {
        bindings.push_back(QRhiShaderResourceBinding::bufferLoadStore(b.m_binding,
                                                                      stages|QRhiShaderResourceBinding::ComputeStage,
                                                                      nullptr));
    }

    return bindings;
}

std::vector<QRhiShaderResourceBinding> PipelineUBOSet::resourceBindings(const RenderCommand &command)
{
    RHITextureManager *textureManager = m_resourceManagers->rhiTextureManager();
    RHIShader *shader = command.m_rhiShader;
    const QRhiShaderResourceBinding::StageFlags stages = QRhiShaderResourceBinding::VertexStage|QRhiShaderResourceBinding::FragmentStage;
    std::vector<QRhiShaderResourceBinding> bindings = {
        QRhiShaderResourceBinding::uniformBuffer(0, stages, m_rvUBO.buffer->rhiBuffer()),
    };

    const size_t dToCmd = distanceToCommand(command);

    // CommandUBO
    {
        const HRHIBuffer &commandUBO = m_commandsUBO.bufferForCommand(dToCmd);
        bindings.push_back(
                    QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(1,
                                                                              stages,
                                                                              commandUBO->rhiBuffer(),
                                                                              int(m_commandsUBO.alignedBlockSize)));
    }

    // Create additional empty UBO Buffer for UBO with binding point > 1 (since
    // we assume 0 and 1 and for Qt3D standard values)
    // Note: if a Parameter provides a UBO Buffer, its content will have been
    // copied at right offset into the matching materials UBO
    for (const MultiUBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
        const HRHIBuffer &materialUBO = ubo.bufferForCommand(dToCmd);
        bindings.push_back(
                    QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(
                        ubo.binding, stages, materialUBO->rhiBuffer(), int(ubo.alignedBlockSize)));
    }

    // Samplers
    const std::vector<ShaderAttribute> &shaderSamplers = shader->samplers();
    std::vector<int> samplersSetIds;
    for (const ShaderParameterPack::NamedResource &textureParameter : command.m_parameterPack.textures()) {
        const HRHITexture &handle = textureManager->getOrAcquireHandle(textureParameter.nodeId);
        const RHITexture *textureData = handle.data();

        for (const ShaderAttribute &samplerAttribute : shaderSamplers) {
            if (samplerAttribute.m_nameId == textureParameter.glslNameId) {
                const auto rhiTexture = textureData->getRhiTexture();
                const auto rhiSampler = textureData->getRhiSampler();
                if (rhiTexture && rhiSampler) {
                    bindings.push_back(QRhiShaderResourceBinding::sampledTexture(
                                           samplerAttribute.m_location,
                                           stages, rhiTexture, rhiSampler));
                    samplersSetIds.push_back(samplerAttribute.m_nameId);
                }
            }
        }
    }

    // Some backends (Vulkan) expect the number of bindings to be exactly the
    // same between the set used to create the pipeline in resourceLayout and
    // the set used to draw with said pipeline generated here.
    // This can be problematic if no parameter is provided for that resource
    const std::vector<ShaderAttribute> unsetSamplers = [&] () {
        std::vector<ShaderAttribute> unset;
        for (const ShaderAttribute &attr : shaderSamplers) {
            const bool matchingSetNameIdFound =
                    std::find_if(samplersSetIds.begin(),
                                 samplersSetIds.end(),
                                 [&attr] (int nameId) {
                return attr.m_nameId == nameId;
            }) != samplersSetIds.end();
            if (!matchingSetNameIdFound)
                unset.push_back(attr);
        }
        return unset;
    }();

    // Generate an empty binding for unset samplers
    const int envLightIrradianceNameId = StringToInt::lookupId(QStringLiteral("envLight_irradiance"));
    const int envLightSpecularNameId = StringToInt::lookupId(QStringLiteral("envLight_specular"));
    for (const ShaderAttribute &sampler : unsetSamplers) {
        if (sampler.m_nameId != envLightSpecularNameId && sampler.m_nameId != envLightIrradianceNameId)
            qCWarning(Backend) << "Sampler" << sampler.m_name << "wasn't set on material. Rendering might not work as expected";
    }

    // SSBO
    for (const BlockToSSBO &ssbo : command.m_parameterPack.shaderStorageBuffers()) {
        RHIBuffer *buffer = m_resourceManagers->rhiBufferManager()->lookupResource(ssbo.m_bufferID);
        if (buffer) {
            bindings.push_back(QRhiShaderResourceBinding::bufferLoadStore(
                                   ssbo.m_bindingIndex,
                                   stages|QRhiShaderResourceBinding::ComputeStage,
                                   buffer->rhiBuffer()));
        }
    }

    return bindings;
}

void PipelineUBOSet::uploadUBOs(SubmissionContext *ctx, RenderView *rv)
{
    // Update UBO data for RV and RC data
    m_rvUBO.buffer->update(QByteArray::fromRawData(reinterpret_cast<const char *>(rv->renderViewUBO()),
                                                   sizeof(RenderViewUBO)));
    int distance = 0;
    for (const RenderCommand *command : m_renderCommands) {
        uploadUBOsForCommand(*command, distance);
        ++distance;
    }

    // Trigger actual upload to GPU
    m_rvUBO.buffer->bind(ctx, RHIBuffer::UniformBuffer);
    for (const HRHIBuffer &ubo : m_commandsUBO.buffers)
        ubo->bind(ctx, RHIBuffer::UniformBuffer);
    for (const MultiUBOBufferWithBindingAndBlockSize &multiUbo : m_materialsUBOs) {
        for (const HRHIBuffer &ubo : multiUbo.buffers)
            ubo->bind(ctx, RHIBuffer::UniformBuffer);
    }
}

namespace {
/*
void printUpload(const UniformValue &value, const QShaderDescription::BlockVariable &member, int arrayOffset)
{
    switch (member.type) {
    case QShaderDescription::VariableType::Int:
        qDebug() << "Updating" << member.name << "with int data: " << *value.constData<int>()
                 << " (offset: " << member.offset + arrayOffset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Float:
        qDebug() << "Updating" << member.name << "with float data: " << *value.constData<float>()
                 << " (offset: " << member.offset + arrayOffset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Vec2:
        qDebug() << "Updating" << member.name << "with vec2 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << " (offset: " << member.offset
                 << ", size: " << member.size + arrayOffset << ")";
        break;
    case QShaderDescription::VariableType::Vec3:
        qDebug() << "Updating" << member.name << "with vec3 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << " (offset: " << member.offset + arrayOffset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Vec4:
        qDebug() << "Updating" << member.name << "with vec4 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << ", " << value.constData<float>()[3] << " (offset: " << member.offset + arrayOffset
                 << ", size: " << member.size << ")";
        break;
    default:
        qDebug() << "Updating" << member.name << "with data: " << value.constData<char>();
        break;
    }
}
//*/

inline void uploadDataToUBO(const QByteArray rawData,
                            const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *ubo,
                            const RHIShader::UBO_Member &member,
                            size_t distanceToCommand, int arrayOffset = 0)
{
    const HRHIBuffer &buffer = ubo->bufferForCommand(distanceToCommand);
    const size_t localOffset = ubo->localOffsetInBufferForCommand(distanceToCommand);
    buffer->update(rawData, int(localOffset) + member.blockVariable.offset + arrayOffset);
}

QByteArray rawDataForUniformValue(const QShaderDescription::BlockVariable &blockVariable,
                                  const UniformValue &value,
                                  bool requiresCopy)
{
    const QByteArray rawData = requiresCopy
              ? QByteArray(value.constData<char>(), std::min(value.byteSize(), blockVariable.size))
              : QByteArray::fromRawData(value.constData<char>(), std::min(value.byteSize(), blockVariable.size));
    const int matrixStride = blockVariable.matrixStride;
    int arrayStride = blockVariable.arrayStride;
    const int firstArrayDim = !blockVariable.arrayDims.empty() ? blockVariable.arrayDims.first() : 0;

    if (blockVariable.arrayDims.size() > 1)
        qCWarning(Backend) << "Multi Dimension arrays not handled yet";

    if (arrayStride != 0 && matrixStride != 0)
        qCWarning(Backend) << "Arrays of matrices not handled yet";

    // Special cases for arrays
    if (firstArrayDim > 0) {
        // It appears that for a float myArray[8]; stride is reported as being
        // 0 but expected size 128 bytes which means 16 bytes per float
        // In that case we compute the arrayStride ourselves
        if (arrayStride == 0)
            arrayStride = blockVariable.size / firstArrayDim;
        if (arrayStride != 0) {
            QByteArray newRawData(firstArrayDim * arrayStride, '\0');
            const int byteSizePerEntry = value.elementByteSize();
            for (int i = 0, m = std::min(firstArrayDim, value.elementCount()); i < m; ++i) {
                std::memcpy(newRawData.data() + i * arrayStride,
                            rawData.constData() + i * byteSizePerEntry,
                            byteSizePerEntry);
            }
            return newRawData;
        }
    }

    // Special cases for matrices which might have to be aligned to a vec4 stride
    if (matrixStride != 0 && value.byteSize() % matrixStride != 0) {
        // Find number of rows
        const int rows = blockVariable.size / matrixStride;
        QByteArray newRawData = QByteArray(rows * matrixStride, '\0');
        const int dataSizePerRow = value.byteSize() / rows;
        for (int i = 0; i < rows; ++i) {
            std::memcpy(newRawData.data() + i * matrixStride,
                        rawData.constData() + i * dataSizePerRow, dataSizePerRow);
        }
        return newRawData;
    }

    // Note: we currently don't handle array of matrices

    return rawData;
}

void uploadUniform(const PackUniformHash &uniforms,
                   const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *ubo,
                   const RHIShader::UBO_Member &member,
                   size_t distanceToCommand, int arrayOffset = 0)
{
    if (!uniforms.contains(member.nameId))
        return;

    const UniformValue &value = uniforms.value(member.nameId);
    // Textures / Images / Buffers can't be UBO members
    if (value.valueType() != UniformValue::ScalarValue)
        return;

    // We can avoid the copies since the raw data copes from the UniformValue
    // which remain alive until the frame has been submitted
    const bool requiresCopy = false;
    const QByteArray rawData = rawDataForUniformValue(member.blockVariable,
                                                      value,
                                                      requiresCopy);
    uploadDataToUBO(rawData, ubo, member, distanceToCommand, arrayOffset);

    // printUpload(value, member.blockVariable, arrayOffset);
}

} // anonymous

void PipelineUBOSet::uploadShaderDataProperty(const ShaderData *shaderData,
                              const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *ubo,
                              const RHIShader::UBO_Member &uboMemberInstance,
                              size_t distanceToCommand, int arrayOffset)
{
    const std::vector<RHIShader::UBO_Member> &structMembers = uboMemberInstance.structMembers;
    const QHash<QString, ShaderData::PropertyValue> &properties = shaderData->properties();
    const int structBaseOffset = uboMemberInstance.blockVariable.offset;
    for (const RHIShader::UBO_Member &member : structMembers) {
        // TO DO: Use nameIds instead
        const auto it = properties.find(member.blockVariable.name);
        if (it != properties.end()) {
            const ShaderData::PropertyValue &value = *it;
            if (value.isNode) {
                // Nested ShaderData
                const ShaderData *child = m_nodeManagers->shaderDataManager()->lookupResource(value.value.value<Qt3DCore::QNodeId>());
                if (child)
                    uploadShaderDataProperty(child, ubo, member,
                                             distanceToCommand,
                                             structBaseOffset + arrayOffset);
                continue;
            }
            if (value.isTransformed) {
                // TO DO: Handle this
                qWarning() << "ShaderData transformed properties not handled yet";
                // QVariant transformedValue = shaderData->getTransformedProperty(&value, viewMatrix);
            }

            // Value is a Scalar or a Scalar Array
            const UniformValue v = UniformValue::fromVariant(value.value);
            Q_ASSERT(v.valueType() == UniformValue::ScalarValue);

            // We have to make a copy here
            const bool requiresCopy = true;
            const QByteArray rawData = rawDataForUniformValue(member.blockVariable,
                                                              v,
                                                              requiresCopy);
            uploadDataToUBO(rawData, ubo, member, distanceToCommand, structBaseOffset + arrayOffset);

            // printUpload(v, member.blockVariable, structBaseOffset + arrayOffset);
        }
    }
}

void PipelineUBOSet::uploadUBOsForCommand(const RenderCommand &command,
                                          const size_t distanceToCommand)
{
    Q_ASSERT(m_nodeManagers);
    RHIShader *shader = command.m_rhiShader;
    if (!shader)
        return;

    {
        const HRHIBuffer &commandUBOBuffer = m_commandsUBO.bufferForCommand(distanceToCommand);
        const size_t localOffset = m_commandsUBO.localOffsetInBufferForCommand(distanceToCommand);
        commandUBOBuffer->update(QByteArray::fromRawData(
                                     reinterpret_cast<const char *>(&command.m_commandUBO),
                                     sizeof(CommandUBO)),
                                 int(localOffset));
    }

    const std::vector<RHIShader::UBO_Block> &uboBlocks = shader->uboBlocks();
    const ShaderParameterPack &parameterPack = command.m_parameterPack;
    const PackUniformHash &uniforms = parameterPack.uniforms();

    // Update Buffer CPU side data based on uniforms being set

    auto findMaterialUBOForBlock = [this] (const RHIShader::UBO_Block &uboBlock)
            -> PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize* {
        auto it = std::find_if(m_materialsUBOs.begin(), m_materialsUBOs.end(),
                               [&uboBlock] (const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize &materialUBO) {
            return materialUBO.binding == uboBlock.block.m_binding;
        });

        if (it == m_materialsUBOs.end())
            return nullptr;
        return &*it;
    };

    auto findUboBlockForBinding = [&uboBlocks] (int blockBinding) -> const RHIShader::UBO_Block * {
        auto it = std::find_if(uboBlocks.begin(), uboBlocks.end(), [&blockBinding] (const RHIShader::UBO_Block &block) {
                return block.block.m_binding == blockBinding;
        });
        if (it == uboBlocks.end())
            return nullptr;
        return &*it;
    };

    // Scalar / Texture Parameter to UBO
    for (const RHIShader::UBO_Block &uboBlock : uboBlocks) {

        // No point in trying to update Bindings 0 or 1 which are reserved
        // for Qt3D default uniforms
        const ShaderUniformBlock &block = uboBlock.block;
        if (block.m_binding <= 1)
            continue;

        // Update UBO with uniform value
        const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *ubo = findMaterialUBOForBlock(uboBlock);
        if (ubo == nullptr)
            continue;

        for (const RHIShader::UBO_Member &member : std::as_const(uboBlock.members)) {
            const QShaderDescription::BlockVariable &blockVariable = member.blockVariable;

            // Array
            if (!blockVariable.arrayDims.empty()) {
                if (!blockVariable.structMembers.empty()) { // Array of structs
                    // we treat structMembers as arrayMembers when we are dealing with an array of structs´
                    const size_t arr0 = size_t(blockVariable.arrayDims[0]);
                    const size_t m = std::max(arr0, member.structMembers.size());
                    for (size_t i = 0; i < m; ++i) {
                        const RHIShader::UBO_Member &arrayMember = member.structMembers[i];
                        for (const RHIShader::UBO_Member &arrayStructMember : arrayMember.structMembers) {
                            uploadUniform(uniforms, ubo,
                                          arrayStructMember,
                                          distanceToCommand,
                                          int(i * blockVariable.size / arr0));
                        }
                    }
                } else { // Array of scalars
                    uploadUniform(uniforms, ubo,
                                  member, distanceToCommand);
                }
            } else {
                if (!blockVariable.structMembers.empty()) { // Struct
                    for (const RHIShader::UBO_Member &structMember : member.structMembers) {
                        uploadUniform(uniforms, ubo,
                                      structMember,
                                      distanceToCommand);
                    }
                } else { // Scalar
                    uploadUniform(uniforms, ubo,
                                  member, distanceToCommand);
                }
            }
        }
    }

    // User provided UBO
    // Since we create per Material UBO, if we directly provide a UBO as a parameter
    // we will have to copy content of the parameter's UBO into the Material UBO at the
    // property offset
    for (const BlockToUBO &ubo : parameterPack.uniformBuffers()) {
        // Find RHIShader::UBO_Block for UBO
        const RHIShader::UBO_Block *block = findUboBlockForBinding(ubo.m_bindingIndex);
        if (block == nullptr)
            continue;
        // Find Material UBO for Block
        PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *materialsUBO = findMaterialUBOForBlock(*block);
        if (materialsUBO == nullptr)
            continue;

        // Copy content of UBO buffer to materialUBO at offset
        Buffer *uboBuffer = m_nodeManagers->bufferManager()->lookupResource(ubo.m_bufferID);
        if (!uboBuffer)
            continue;

        const HRHIBuffer &materialBuffer = materialsUBO->bufferForCommand(distanceToCommand);
        const size_t localOffsetIntoBuffer = materialsUBO->localOffsetInBufferForCommand(distanceToCommand);
        materialBuffer->update(uboBuffer->data(), int(localOffsetIntoBuffer));
    }

    // ShaderData -> convenience for filling a struct member of a UBO
    // Note: we could use setDefaultUniformBlockShaderDataValue to unpack all values as individual uniforms
    // but setting the whole ShaderData properties at once is a lot more efficient
    for (const ShaderDataForUBO &shaderDataForUbo : parameterPack.shaderDatasForUBOs()) {
        // Find ShaderData backend
        const ShaderData *shaderData = m_nodeManagers->shaderDataManager()->lookupResource(shaderDataForUbo.m_shaderDataID);
        if (shaderData == nullptr)
            continue;
        // Find RHIShader::UBO_Block for ShaderData
        const RHIShader::UBO_Block *block = findUboBlockForBinding(shaderDataForUbo.m_bindingIndex);
        if (block == nullptr)
            continue;
        // Find Material UBO for Block
        PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *materialsUBO = findMaterialUBOForBlock(*block);
        if (materialsUBO == nullptr)
            continue;

        // Upload ShaderData property that match members of each UBO block instance
        for (const RHIShader::UBO_Member &uboInstance : std::as_const(block->members)) {
            uploadShaderDataProperty(shaderData, materialsUBO,
                                     uboInstance, distanceToCommand);
        }
    }

    // Note: There's nothing to do for SSBO as those are directly uploaded to the GPU, no extracting
    // required
}

HRHIBuffer PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize::bufferForCommand(size_t distanceToCommand) const
{
    const size_t uboIdx = distanceToCommand / commandsPerUBO;
    return buffers[uboIdx];
}

size_t PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize::localOffsetInBufferForCommand(size_t distanceToCommand) const
{
    return (distanceToCommand % commandsPerUBO) * alignedBlockSize;
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
