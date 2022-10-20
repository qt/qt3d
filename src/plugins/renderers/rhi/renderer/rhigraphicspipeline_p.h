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

#include <private/qrhi_p.h>
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
