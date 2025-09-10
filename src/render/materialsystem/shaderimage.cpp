// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "shaderimage_p.h"
#include <Qt3DRender/private/qshaderimage_p.h>
#include <Qt3DRender/qabstracttexture.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

ShaderImage::ShaderImage()
    : BackendNode(BackendNode::ReadOnly)
    , m_textureId()
    , m_mipLevel(0)
    , m_layer(0)
    , m_layered(false)
    , m_access(QShaderImage::ReadWrite)
    , m_format(QShaderImage::NoFormat)
{
}

void ShaderImage::cleanup()
{
    QBackendNode::setEnabled(false);
    m_textureId = Qt3DCore::QNodeId();
    m_mipLevel = 0;
    m_layer = 0;
    m_layered = false;
    m_access = QShaderImage::ReadWrite;
    m_format = QShaderImage::NoFormat;
}

void ShaderImage::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QShaderImage *node = qobject_cast<const QShaderImage *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    const Qt3DCore::QNodeId textureNodeId = Qt3DCore::qIdForNode(node->texture());
    if (textureNodeId != m_textureId) {
        m_textureId = textureNodeId;
        markDirty(AbstractRenderer::ParameterDirty);
    }

    if (node->mipLevel() != m_mipLevel) {
        m_mipLevel = node->mipLevel();
        markDirty(AbstractRenderer::ParameterDirty);
    }

    if (node->layer() != m_layer) {
        m_layer = node->layer();
        markDirty(AbstractRenderer::ParameterDirty);
    }

    if (node->layered() != m_layered) {
        m_layered = node->layered();
        markDirty(AbstractRenderer::ParameterDirty);
    }

    if (node->format() != m_format) {
        m_format = node->format();
        markDirty(AbstractRenderer::ParameterDirty);
    }
    if (node->access() != m_access) {
        m_access = node->access();
        markDirty(AbstractRenderer::ParameterDirty);
    }

}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
