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

#include "pipelineuboset_p.h"
#include <rendercommand_p.h>
#include <renderview_p.h>
#include <rhishader_p.h>
#include <rhibuffer_p.h>
#include <shaderparameterpack_p.h>
#include <shadervariables_p.h>
#include <rhigraphicspipeline_p.h>
#include <rhiresourcemanagers_p.h>
#include <submissioncontext_p.h>
#include <QtGui/private/qrhi_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

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

void PipelineUBOSet::initializeLayout(SubmissionContext *ctx, RHIShader *shader)
{
    // We should only be called with a clean Pipeline
    Q_ASSERT(m_rvUBO.buffer.isNull());
    RHIBufferManager *bufferManager = m_resourceManagers->rhiBufferManager();

    m_rvUBO.binding = 0;
    m_rvUBO.blockSize = sizeof(RenderViewUBO);
    m_rvUBO.buffer = bufferManager->allocateResource();

    m_commandsUBO.binding = 1;
    m_commandsUBO.blockSize = sizeof(CommandUBO);
    m_commandsUBO.alignedBlockSize = ctx->rhi()->ubufAligned((m_commandsUBO.blockSize));
    m_commandsUBO.buffer = bufferManager->allocateResource();

    const std::vector<ShaderUniformBlock> &uniformBlocks = shader->uniformBlocks();
    for (const ShaderUniformBlock &block : uniformBlocks) {
        if (block.m_binding > 1) { // Binding 0 and 1 are for RV and Command UBOs
            m_materialsUBOs.push_back(
                        { block.m_binding,
                          block.m_size,
                          size_t(ctx->rhi()->ubufAligned(block.m_size)),
                          bufferManager->allocateResource()});
        }
    }
}

void PipelineUBOSet::releaseResources()
{
    Q_ASSERT(m_resourceManagers);
    RHIBufferManager *bufferManager = m_resourceManagers->rhiBufferManager();

    bufferManager->release(m_rvUBO.buffer);
    bufferManager->release(m_commandsUBO.buffer);

    m_rvUBO = {};
    m_commandsUBO = {};

    if (!m_materialsUBOs.empty()) {
        for (const UBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
            bufferManager->release(ubo.buffer);
        }
        m_materialsUBOs.clear();
    }
}

bool PipelineUBOSet::allocateUBOs(SubmissionContext *ctx)
{
    // Note: RHIBuffer only releases/recreates when it needs more size than what it
    // can currently hold
    Q_ASSERT(m_resourceManagers);
    const bool dynamic = true;
    const size_t commandCount = std::max(m_renderCommands.size(), size_t(1));

    // RHIBuffer only reallocates if size is < than required
    m_rvUBO.buffer->allocate(QByteArray(m_rvUBO.blockSize, '\0'), dynamic);\

    // We need to take into account any minimum alignment requirement for dynamic offsets
    m_commandsUBO.buffer->allocate(QByteArray(m_commandsUBO.alignedBlockSize * commandCount, '\0'), dynamic);

    // Binding buffer ensure underlying RHI resource is created
    m_rvUBO.buffer->bind(ctx, RHIBuffer::UniformBuffer);
    m_commandsUBO.buffer->bind(ctx, RHIBuffer::UniformBuffer);

    for (const UBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
        if (ubo.binding > 1) { // Binding 0 and 1 are for RV and Command UBOs
            ubo.buffer->allocate(QByteArray(ubo.alignedBlockSize * commandCount, '\0'), dynamic);
            ubo.buffer->bind(ctx, RHIBuffer::UniformBuffer);
        }
    }

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
    offsets.push_back({1, dToCmd * m_commandsUBO.alignedBlockSize});

    for (const UBOBufferWithBindingAndBlockSize &buffer : m_materialsUBOs)
        offsets.push_back({buffer.binding, dToCmd * buffer.alignedBlockSize});

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
    for (const UBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs)
        bindings.push_back(
                    QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(
                        ubo.binding, stages, nullptr, ubo.blockSize));

    // Samplers
    for (const ShaderAttribute &samplerAttribute : shader->samplers()) {
        bindings.push_back(QRhiShaderResourceBinding::sampledTexture(
                               samplerAttribute.m_location,
                               stages, nullptr, nullptr));
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
        QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(1, stages, m_commandsUBO.buffer->rhiBuffer(), m_commandsUBO.buffer->size())
    };

    // TO DO: Handle Parameters that directly define a UBO or SSBO
    //    const auto &blockToUBOs = parameterPack.uniformBuffers();
    //    const auto &blockToSSBOs = parameterPack.shaderStorageBuffers();

    // Create additional empty UBO Buffer for UBO with binding point > 1 (since
    // we assume 0 and 1 and for Qt3D standard values)
    for (const UBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs)
        bindings.push_back(
                    QRhiShaderResourceBinding::uniformBufferWithDynamicOffset(
                        ubo.binding, stages, ubo.buffer->rhiBuffer(), ubo.buffer->size()));

    // Samplers
    for (const ShaderParameterPack::NamedResource &textureParameter : command.m_parameterPack.textures()) {
        const HRHITexture &handle = textureManager->getOrAcquireHandle(textureParameter.nodeId);
        const RHITexture *textureData = handle.data();

        for (const ShaderAttribute &samplerAttribute : shader->samplers()) {
            if (samplerAttribute.m_nameId == textureParameter.glslNameId) {
                const auto rhiTexture = textureData->getRhiTexture();
                const auto rhiSampler = textureData->getRhiSampler();
                if (rhiTexture && rhiSampler) {
                    bindings.push_back(QRhiShaderResourceBinding::sampledTexture(
                                           samplerAttribute.m_location,
                                           stages, rhiTexture, rhiSampler));
                }
            }
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
    m_commandsUBO.buffer->bind(ctx, RHIBuffer::UniformBuffer);
    for (const UBOBufferWithBindingAndBlockSize &ubo : m_materialsUBOs) {
        // Binding triggers the upload
        ubo.buffer->bind(ctx, RHIBuffer::UniformBuffer);
    }
}

namespace {
void printUpload(const UniformValue &value, const QShaderDescription::BlockVariable &member)
{
    switch (member.type) {
    case QShaderDescription::VariableType::Int:
        qDebug() << "Updating" << member.name << "with int data: " << *value.constData<int>()
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Float:
        qDebug() << "Updating" << member.name << "with float data: " << *value.constData<float>()
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Vec2:
        qDebug() << "Updating" << member.name << "with vec2 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << " (offset: " << member.offset
                 << ", size: " << member.size << ")";
        ;
        break;
    case QShaderDescription::VariableType::Vec3:
        qDebug() << "Updating" << member.name << "with vec3 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        ;
        break;
    case QShaderDescription::VariableType::Vec4:
        qDebug() << "Updating" << member.name << "with vec4 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << ", " << value.constData<float>()[3] << " (offset: " << member.offset
                 << ", size: " << member.size << ")";
        ;
        break;
    default:
        qDebug() << "Updating" << member.name << "with data: " << value.constData<char>();
        break;
    }
}

void uploadUniform(const PackUniformHash &uniforms,
                   const PipelineUBOSet::UBOBufferWithBindingAndBlockSize &ubo,
                   const RHIShader::UBO_Member &member,
                   size_t distanceToCommand, int arrayOffset = 0)
{
    if (!uniforms.contains(member.nameId))
        return;

    const UniformValue &value = uniforms.value(member.nameId);

    const QByteArray rawData = QByteArray::fromRawData(value.constData<char>(), std::min(value.byteSize(), member.blockVariable.size));
    ubo.buffer->update(rawData, ubo.alignedBlockSize * distanceToCommand + member.blockVariable.offset + arrayOffset);

//    printUpload(value, member);
}

} // anonymous

void PipelineUBOSet::uploadUBOsForCommand(const RenderCommand &command,
                                          const size_t distanceToCommand)
{
    RHIShader *shader = command.m_rhiShader;
    if (!shader)
        return;

    m_commandsUBO.buffer->update(QByteArray::fromRawData(
                                     reinterpret_cast<const char *>(&command.m_commandUBO),
                                     sizeof(CommandUBO)),
                                 distanceToCommand * m_commandsUBO.alignedBlockSize);

    const std::vector<RHIShader::UBO_Block> &uboBlocks = shader->uboBlocks();
    const ShaderParameterPack &parameterPack = command.m_parameterPack;
    const PackUniformHash &uniforms = parameterPack.uniforms();

    // Update Buffer CPU side data based on uniforms being set
    for (const RHIShader::UBO_Block &uboBlock : uboBlocks) {

        // No point in trying to update Bindings 0 or 1 which are reserved
        // for Qt3D default uniforms
        const ShaderUniformBlock &block = uboBlock.block;
        if (block.m_binding <= 1)
            continue;

        // Update UBO with uniform value
        auto it = std::find_if(m_materialsUBOs.begin(), m_materialsUBOs.end(), [&block] (const PipelineUBOSet::UBOBufferWithBindingAndBlockSize &buffer) {
            return buffer.binding == block.m_binding;
        });

        if (it == m_materialsUBOs.end())
            continue;

        const PipelineUBOSet::UBOBufferWithBindingAndBlockSize &ubo = *it;

        for (const RHIShader::UBO_Member &member : qAsConst(uboBlock.members)) {
            const QShaderDescription::BlockVariable &blockVariable = member.blockVariable;
            if (!blockVariable.arrayDims.empty()) {
                if (!blockVariable.structMembers.empty()) {
                    const int arr0 = blockVariable.arrayDims[0];
                    for (int i = 0; i < arr0; i++) {
                        for (const RHIShader::UBO_Member &structMember : member.structMembers) {
                            uploadUniform(uniforms, ubo,
                                          structMember,
                                          distanceToCommand,
                                          i * blockVariable.size / arr0);
                        }
                    }
                } else {
                    uploadUniform(uniforms, ubo,
                                  member, distanceToCommand);
                }
            } else {
                uploadUniform(uniforms, ubo,
                              member, distanceToCommand);
            }
        }
    }
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
