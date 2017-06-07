/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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
