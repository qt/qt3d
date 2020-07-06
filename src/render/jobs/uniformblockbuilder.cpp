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

#include "uniformblockbuilder_p.h"
#include <QString>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

namespace {

const QString blockArray = QStringLiteral("[%1]");
const int qNodeIdTypeId = qMetaTypeId<QNodeId>();

}

UniformBlockValueBuilder::UniformBlockValueBuilder(
        const std::vector<int> &uniformNamesIds,
        ShaderDataManager *shaderDataManager,
        TextureManager *textureManager,
        const Matrix4x4 &matrix)
    : m_uniformNamesIds(uniformNamesIds)
    , m_shaderDataManager(shaderDataManager)
    , m_textureManager(textureManager)
    , m_viewMatrix(matrix)
{
}

void UniformBlockValueBuilder::buildActiveUniformNameValueMapHelper(const ShaderData *currentShaderData,
                                                                    const QString &blockName,
                                                                    const int propertyInBlockNameId,
                                                                    const int propertyNameId,
                                                                    const ShaderData::PropertyValue *value)
{
    // In the end, values are either scalar or a scalar array
    // Composed elements (structs, structs array) are simplified into simple scalars
    if (value->isArray) { // Array
        const QVariantList list = value->value.value<QVariantList>();
        if (value->isNode) { // Array of struct qmlPropertyName[i].structMember
            for (int i = 0; i < list.size(); ++i) {
                const QVariant variantElement = list.at(i);
                if (list.at(i).userType() == qNodeIdTypeId) {
                    const auto nodeId = variantElement.value<QNodeId>();
                    ShaderData *subShaderData = m_shaderDataManager->lookupResource(nodeId);
                    if (subShaderData) {
                        buildActiveUniformNameValueMapStructHelper(subShaderData,
                                                                   blockName + QLatin1Char('.') + StringToInt::lookupString(propertyNameId) + blockArray.arg(i));
                    }
                    // Note: we only handle ShaderData as nested container nodes here
                }
            }
        } else { // Array of scalar/vec  qmlPropertyName[0]
            if (Qt3DCore::contains(m_uniformNamesIds, propertyInBlockNameId)) {
                activeUniformNamesToValue.insert(propertyInBlockNameId, value->value);
            }
        }
    } else if (value->isNode) { // Struct qmlPropertyName.structMember
        const auto nodeId = value->value.value<QNodeId>();
        ShaderData *rSubShaderData = m_shaderDataManager->lookupResource(nodeId);
        if (rSubShaderData) {
            buildActiveUniformNameValueMapStructHelper(rSubShaderData,
                                                       blockName,
                                                       StringToInt::lookupString(propertyNameId));
        } else if (m_textureManager->contains(nodeId)) {
            activeUniformNamesToValue.insert(propertyInBlockNameId, value->value);
        }
    } else { // Scalar / Vec
        if (Qt3DCore::contains(m_uniformNamesIds, propertyInBlockNameId)) {
            // If the property needs to be transformed, we transform it here as
            // the shaderdata cannot hold transformed properties for multiple
            // thread contexts at once
            activeUniformNamesToValue.insert(propertyInBlockNameId,
                                             currentShaderData->getTransformedProperty(value, m_viewMatrix));
        }
    }
}

void UniformBlockValueBuilder::buildActiveUniformNameValueMapStructHelper(ShaderData *rShaderData,
                                                                          const QString &blockName,
                                                                          const QString &qmlPropertyName)
{
    QString fullBlockName;
    fullBlockName.reserve(blockName.length() + 1 + qmlPropertyName.length());
    fullBlockName.append(blockName);
    if (!qmlPropertyName.isEmpty()) {
        fullBlockName.append(QLatin1String("."));
        fullBlockName.append(qmlPropertyName);
    }

    // Retrieve set of {NameId -> PropertyValue} for Block
    const int fullBlockNameId = StringToInt::lookupId(fullBlockName);
    if (!rShaderData->hasPropertyValuesForBlock(fullBlockNameId))
        rShaderData->generatePropertyValuesForBlock(fullBlockName);
    const ShaderData::PropertyValuesForBlock &propertiesForBlock = rShaderData->propertyValuesForBlock(fullBlockNameId);

    for (const auto &nameIdPropertyPair : propertiesForBlock) {
        buildActiveUniformNameValueMapHelper(rShaderData,
                                             fullBlockName,
                                             // Block.Property Name
                                             std::get<0>(nameIdPropertyPair),
                                             // Property Name
                                             std::get<1>(nameIdPropertyPair),
                                             // PropertyValue
                                             std::get<2>(nameIdPropertyPair));
    }
}


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
