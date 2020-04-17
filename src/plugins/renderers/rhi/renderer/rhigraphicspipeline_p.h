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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class RHIBuffer;

class RHIGraphicsPipeline
{
public:
    struct UBOBuffer
    {
        HRHIBuffer handle {};
        RHIBuffer *buffer {};
    };

    RHIGraphicsPipeline();
    ~RHIGraphicsPipeline();

    QRhiBuffer *commandUBO() const { return m_cmdUbo; }
    QRhiBuffer *renderViewUBO() const { return m_rvUbo; }
    QRhiGraphicsPipeline *pipeline() const { return m_pipeline; }
    QRhiShaderResourceBindings *shaderResourceBindings() const { return m_shaderResourceBindings; }
    QHash<int, UBOBuffer> ubos() const { return m_ubos; }
    int score() const { return m_score; }

    void setPipeline(QRhiGraphicsPipeline *pipeline) { m_pipeline = pipeline; }
    void setCommandUBO(QRhiBuffer *commandUBO) { m_cmdUbo = commandUBO; }
    void setRenderViewUBO(QRhiBuffer *rvUBO) { m_rvUbo = rvUBO; }
    void setShaderResourceBindings(QRhiShaderResourceBindings *shaderResourceBindings)
    {
        m_shaderResourceBindings = shaderResourceBindings;
    }
    void setUBOs(const QHash<int, UBOBuffer> ubos) { m_ubos = ubos; }

    void setAttributesToBindingHash(const QHash<int, int> &attributeNameToBinding)
    {
        m_attributeNameIdToBindingIndex = attributeNameToBinding;
    }
    int bindingIndexForAttribute(int attributeNameId) const
    {
        return m_attributeNameIdToBindingIndex.value(attributeNameId, -1);
    }
    void increaseScore() { ++m_score; }
    void decreaseScore() { --m_score; }

    void cleanup();

private:
    QRhiBuffer *m_rvUbo;
    QRhiBuffer *m_cmdUbo;
    QRhiGraphicsPipeline *m_pipeline;
    QRhiShaderResourceBindings *m_shaderResourceBindings;
    // For user defined uniforms
    QHash<int, UBOBuffer> m_ubos;
    QHash<int, int> m_attributeNameIdToBindingIndex;
    int m_score;
};

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIGRAPHICSPIPELINE_H
