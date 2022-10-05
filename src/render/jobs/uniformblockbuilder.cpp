// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    fullBlockName.reserve(blockName.size() + 1 + qmlPropertyName.size());
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
