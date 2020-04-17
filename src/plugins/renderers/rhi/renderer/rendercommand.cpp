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

#include "rendercommand_p.h"
#include "renderer/rhigraphicspipeline_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

RenderCommand::RenderCommand()
    : m_rhiShader(nullptr),
      m_stateSet(nullptr),
      m_depth(0.0f),
      m_changeCost(0),
      m_type(RenderCommand::Draw),
      m_workGroups(),
      m_primitiveCount(0),
      m_primitiveType(QGeometryRenderer::Triangles),
      m_restartIndexValue(-1),
      m_firstInstance(0),
      m_firstVertex(0),
      m_verticesPerPatch(0),
      m_instanceCount(0),
      m_indexOffset(0),
      m_indexAttributeByteOffset(0),
      m_indexAttributeDataType(Qt3DRender::QAttribute::UnsignedShort),
      m_indirectAttributeByteOffset(0),
      m_drawIndexed(false),
      m_drawIndirect(false),
      m_primitiveRestartEnabled(false),
      m_isValid(false),
      indexAttribute(nullptr),
      indexBuffer(nullptr),
      m_commandUBO(),
      pipeline(nullptr)

{
    m_workGroups[0] = 0;
    m_workGroups[1] = 0;
    m_workGroups[2] = 0;
}

bool RenderCommand::isValid() const noexcept
{
    return m_rhiShader && pipeline && pipeline->pipeline();
}

bool operator==(const RenderCommand &a, const RenderCommand &b) noexcept
{
    return (a.m_rhiShader == b.m_rhiShader && a.m_material == b.m_material
            && a.m_stateSet == b.m_stateSet && a.m_geometry == b.m_geometry
            && a.m_geometryRenderer == b.m_geometryRenderer
            && a.m_indirectDrawBuffer == b.m_indirectDrawBuffer
            && a.m_activeAttributes == b.m_activeAttributes && a.m_depth == b.m_depth
            && a.m_changeCost == b.m_changeCost && a.m_shaderId == b.m_shaderId
            && a.m_workGroups[0] == b.m_workGroups[0] && a.m_workGroups[1] == b.m_workGroups[1]
            && a.m_workGroups[2] == b.m_workGroups[2] && a.m_primitiveCount == b.m_primitiveCount
            && a.m_primitiveType == b.m_primitiveType
            && a.m_restartIndexValue == b.m_restartIndexValue
            && a.m_firstInstance == b.m_firstInstance && a.m_firstVertex == b.m_firstVertex
            && a.m_verticesPerPatch == b.m_verticesPerPatch
            && a.m_instanceCount == b.m_instanceCount && a.m_indexOffset == b.m_indexOffset
            && a.m_indexAttributeByteOffset == b.m_indexAttributeByteOffset
            && a.m_drawIndexed == b.m_drawIndexed && a.m_drawIndirect == b.m_drawIndirect
            && a.m_primitiveRestartEnabled == b.m_primitiveRestartEnabled
            && a.m_isValid == b.m_isValid && a.m_computeCommand == b.m_computeCommand);
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
