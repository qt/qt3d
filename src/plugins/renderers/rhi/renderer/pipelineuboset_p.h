// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_PIPELINEUBOSET_H
#define QT3DRENDER_RENDER_RHI_PIPELINEUBOSET_H

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

#include <rhihandle_types_p.h>
#include <rhi/qrhi.h>
#include <shadervariables_p.h>
#include <rhishader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class ShaderData;
class NodeManagers;

namespace Rhi {

class RHIShader;
class RHIBuffer;
class RenderCommand;
class SubmissionContext;
class RenderView;
class RHIResourceManagers;

class PipelineUBOSet
{
public:
    struct UBOBufferWithBindingAndBlockSize
    {
        int binding = -1;
        int blockSize = -1;
        size_t alignedBlockSize = 0;
        HRHIBuffer buffer;
    };

    struct MultiUBOBufferWithBindingAndBlockSize
    {
        int binding = -1;
        int blockSize = -1;
        size_t alignedBlockSize = 0;
        size_t alignment = 0;
        size_t commandsPerUBO = 0;
        std::vector<HRHIBuffer> buffers;

        HRHIBuffer bufferForCommand(size_t distanceToCommand) const;
        size_t localOffsetInBufferForCommand(size_t distanceToCommand) const;
    };

    PipelineUBOSet();
    ~PipelineUBOSet();

    void clear();
    void addRenderCommand(const RenderCommand &cmd);

    size_t distanceToCommand(const RenderCommand &command) const;
    std::vector<QRhiCommandBuffer::DynamicOffset> offsets(const RenderCommand &command) const;
    std::vector<QRhiShaderResourceBinding> resourceLayout(const RHIShader *shader);
    std::vector<QRhiShaderResourceBinding> resourceBindings(const RenderCommand &command);

    bool allocateUBOs(SubmissionContext *ctx);
    void uploadUBOs(SubmissionContext *ctx, RenderView *rv);
    void setResourceManager(RHIResourceManagers *managers);
    void setNodeManagers(NodeManagers *manager);
    void initializeLayout(SubmissionContext *ctx, RHIShader *shader);

private:
    void releaseResources();
    void uploadUBOsForCommand(const RenderCommand &command,
                              size_t distanceToCommand);
    void uploadShaderDataProperty(const ShaderData *shaderData,
                                  const PipelineUBOSet::MultiUBOBufferWithBindingAndBlockSize *ubo,
                                  const RHIShader::UBO_Member &uboMemberInstance,
                                  size_t distanceToCommand, int arrayOffset = 0);

    UBOBufferWithBindingAndBlockSize m_rvUBO; // Fixed size
    MultiUBOBufferWithBindingAndBlockSize m_commandsUBO; // Variable size
    std::vector<MultiUBOBufferWithBindingAndBlockSize> m_materialsUBOs; // Variable size
    std::vector<ShaderStorageBlock> m_storageBlocks; // Fixed size

    // TO DO: We also need to handle cases where UBO was directly provided by the frontend
    // API and is not built up from Parameters
    std::vector<const RenderCommand *> m_renderCommands;
    RHIResourceManagers *m_resourceManagers = nullptr;
    NodeManagers *m_nodeManagers = nullptr;

    template<typename Pipeline, typename Key>
    friend class RHIPipelineBase;
};


} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_PIPELINEUBOSET_H
