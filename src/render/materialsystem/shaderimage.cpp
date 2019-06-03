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
#include <Qt3DCore/qpropertyupdatedchange.h>


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

void ShaderImage::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Qt3DCore::QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
    if (e->type() == Qt3DCore::PropertyUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("texture")) {
            m_textureId = propertyChange->value().value<Qt3DCore::QNodeId>();
            markDirty(AbstractRenderer::ParameterDirty);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("mipLevel")) {
            m_mipLevel = propertyChange->value().toInt();
            markDirty(AbstractRenderer::ParameterDirty);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("layer")) {
            m_layer = propertyChange->value().toInt();
            markDirty(AbstractRenderer::ParameterDirty);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("layered")) {
            m_layered = propertyChange->value().toBool();
            markDirty(AbstractRenderer::ParameterDirty);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            m_format = propertyChange->value().value<QShaderImage::ImageFormat>();
            markDirty(AbstractRenderer::ParameterDirty);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("access")) {
            m_access = propertyChange->value().value<QShaderImage::Access>();
            markDirty(AbstractRenderer::ParameterDirty);
        }
    }
    BackendNode::sceneChangeEvent(e);
}

void ShaderImage::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QShaderImageData>>(change);
    const QShaderImageData &data = typedChange->data;

    m_textureId = data.textureId;
    m_mipLevel = data.mipLevel;
    m_layer = data.layer;
    m_layered = data.layered;
    m_access = data.access;
    m_format = data.format;
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
