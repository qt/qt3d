// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RHIGRAPHICSPIPELINE_H
#define QT3DRENDER_RENDER_RHI_RHIGRAPHICSPIPELINE_H

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

#include <rhi/qrhi.h>
#include <rhihandle_types_p.h>
#include <pipelineuboset_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qgeometryrenderer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class RHIBuffer;

// Geometry | Shader | RenderStateMask
struct GraphicsPipelineIdentifier
{
    int geometryLayoutKey;
    Qt3DCore::QNodeId shader;
    Qt3DCore::QNodeId renderTarget;
    Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType;
    int renderStatesKey = 0;
};

struct ComputePipelineIdentifier
{
    Qt3DCore::QNodeId shader;
    int renderViewIndex;
};

template<typename Pipeline, typename Key>
class RHIPipelineBase
{
public:
    virtual ~RHIPipelineBase() {}

    PipelineUBOSet* uboSet() { return &m_uboSet; }

    Pipeline *pipeline() const { return m_pipeline; }
    void setPipeline(Pipeline *pipeline) { m_pipeline = pipeline; }

    void setKey(const Key &key) { m_key = key; }
    Key key() const { return m_key; }

    void setShaderResourceBindings(QRhiShaderResourceBindings *shaderResourceBindings) { m_shaderResourceBindings = shaderResourceBindings; }
    QRhiShaderResourceBindings *shaderResourceBindings() const { return m_shaderResourceBindings; }

    int score() const { return m_score; }
    void increaseScore() { m_score += 2; }
    void decreaseScore() { --m_score; }

    bool isComplete() { return m_complete; }
    void markComplete() { m_complete = true; }

    virtual void cleanup()
    {
        delete m_shaderResourceBindings;
        delete m_pipeline;
        m_pipeline = nullptr;
        m_shaderResourceBindings = nullptr;
        m_uboSet.releaseResources();
        m_uboSet.clear();
        m_key = {};
        m_score = 5;
    }

protected:
    RHIPipelineBase() {}

    Pipeline *m_pipeline = nullptr;
    QRhiShaderResourceBindings *m_shaderResourceBindings = nullptr;
    // For user defined uniforms
    PipelineUBOSet m_uboSet;
    Key m_key = {};
    int m_score = 5;
    int m_complete = false;
};

class RHIGraphicsPipeline : public RHIPipelineBase<QRhiGraphicsPipeline, GraphicsPipelineIdentifier>
{
public:
    void setAttributesToBindingHash(const QHash<int, int> &attributeNameToBinding)
    {
        m_attributeNameIdToBindingIndex = attributeNameToBinding;
    }

    int bindingIndexForAttribute(int attributeNameId) const
    {
        return m_attributeNameIdToBindingIndex.value(attributeNameId, -1);
    }

    virtual void cleanup() override
    {
        RHIPipelineBase<QRhiGraphicsPipeline, GraphicsPipelineIdentifier>::cleanup();
        m_attributeNameIdToBindingIndex.clear();
    }

private:
    // For user defined uniforms
    QHash<int, int> m_attributeNameIdToBindingIndex;
};

class RHIComputePipeline : public RHIPipelineBase<QRhiComputePipeline, ComputePipelineIdentifier>
{
};

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIGRAPHICSPIPELINE_H
