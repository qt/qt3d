// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "shaderdata_p.h"
#include "qshaderdata.h"
#include "qshaderdata_p.h"
#include <QMetaProperty>
#include <QMetaObject>
#include <private/qbackendnode_p.h>
#include <private/managers_p.h>
#include <private/nodemanagers_p.h>
#include <Qt3DRender/private/stringtoint_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

namespace {

const int qNodeIdTypeId = qMetaTypeId<Qt3DCore::QNodeId>();

}

ShaderData::ShaderData()
    : m_managers(nullptr)
{
}

ShaderData::~ShaderData()
{
}

void ShaderData::setManagers(NodeManagers *managers)
{
    m_managers = managers;
}

void ShaderData::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QShaderData *node = qobject_cast<const QShaderData *>(frontEnd);
    if (!node)
        return;
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (firstTime) {
        m_propertyReader = node->propertyReader();
        m_blockNameToPropertyValues.clear();

        const QMetaObject *metaObj = node->metaObject();
        const int propertyOffset = QShaderData::staticMetaObject.propertyOffset();
        const int propertyCount = metaObj->propertyCount();
        // Dynamic properties names
        const auto dynamicPropertyNames = node->dynamicPropertyNames();

        QList<QString> propertyNames;
        propertyNames.reserve(propertyCount - propertyOffset + dynamicPropertyNames.size());

        // Statiically defined properties
        for (int i = propertyOffset; i < propertyCount; ++i) {
            const QMetaProperty pro = metaObj->property(i);
            if (pro.isWritable())
                propertyNames.push_back(QString::fromLatin1(pro.name()));
        }
        // Dynamic properties
        for (const QByteArray &propertyName : dynamicPropertyNames)
            propertyNames.push_back(QString::fromLatin1(propertyName));

        for (const QString &propertyName : propertyNames) {
            if (propertyName == QStringLiteral("data") ||
                    propertyName == QStringLiteral("objectName") ||
                    propertyName == QStringLiteral("childNodes")) // We don't handle default Node properties
                continue;

            const QVariant &propertyValue = m_propertyReader->readProperty(node->property(propertyName.toLatin1()));
            bool isNode = false;
            bool isTransformed = false;
            bool isArray = false;

            // We check if the property is a QNodeId
            isNode = (propertyValue.userType() == qNodeIdTypeId);
            // We check if QList<QNodeId>
            if (propertyValue.userType() == QMetaType::QVariantList) {
                isArray = true;
                QVariantList list = propertyValue.value<QVariantList>();
                if (list.size() > 0 && list.at(0).userType() == qNodeIdTypeId)
                    isNode = true;
            }

            // We check if property is a Transformed property
            QString transformedPropertyName;
            if (propertyValue.userType() == QMetaType::QVector3D) {
                // if there is a matching QShaderData::TransformType propertyTransformed
                transformedPropertyName = propertyName + QLatin1String("Transformed");
                isTransformed = propertyNames.contains(transformedPropertyName);
                if (!isTransformed)
                    transformedPropertyName.clear();
            }
            m_originalProperties.insert(propertyName, { propertyValue, isNode, isArray, isTransformed, transformedPropertyName });
        }
        BackendNode::markDirty(AbstractRenderer::ParameterDirty);
    } else {
        // Updates
        if (!m_propertyReader.isNull()) {
            auto it = m_originalProperties.begin();
            const auto end = m_originalProperties.end();

            while (it != end) {
                const QVariant newValue = m_propertyReader->readProperty(node->property(it.key().toLatin1()));
                PropertyValue &propValue = it.value();
                if (propValue.value != newValue) {
                    // Note we aren't notified about nested QShaderData in this call
                    // only scalar / vec properties
                    propValue.value = newValue;
                    BackendNode::markDirty(AbstractRenderer::ParameterDirty);
                }
                ++it;
            }
        }
    }
}

bool ShaderData::hasPropertyValuesForBlock(int blockName) const
{
    std::shared_lock readLocker(m_lock);
    return m_blockNameToPropertyValues.find(blockName) != m_blockNameToPropertyValues.cend();
}

const ShaderData::PropertyValuesForBlock &ShaderData::propertyValuesForBlock(int blockName) const
{
    std::shared_lock readLocker(m_lock);
    return m_blockNameToPropertyValues.at(blockName);
}

void ShaderData::generatePropertyValuesForBlock(const QString &fullBlockName)
{
    const QHash<QString, ShaderData::PropertyValue> &props = properties();

    ShaderData::PropertyValuesForBlock valueBlock;
    valueBlock.reserve(props.size());

    auto it = props.cbegin();
    const auto end = props.cend();
    while (it != end) {
        QString propertyName = it.key();
        // If we are dealing with a nested value, check if it is an an array
        if (it->isArray && !it->isNode)
            propertyName += QLatin1String("[0]");

        QString fullPropertyName;
        fullPropertyName.reserve(fullBlockName.size() + 1 + it.key().size());
        fullPropertyName.append(fullBlockName);
        fullPropertyName.append(QLatin1String("."));
        fullPropertyName.append(propertyName);

        // We only do this for properties on root level
        valueBlock.push_back({ StringToInt::lookupId(fullPropertyName),
                               StringToInt::lookupId(propertyName),
                               it.operator ->() });
        ++it;
    }

    std::unique_lock writeLocker(m_lock);
    m_blockNameToPropertyValues[StringToInt::lookupId(fullBlockName)] = std::move(valueBlock);
}

ShaderData *ShaderData::lookupResource(NodeManagers *managers, QNodeId id)
{
    return managers->shaderDataManager()->lookupResource(id);
}

ShaderData *ShaderData::lookupResource(QNodeId id)
{
    return ShaderData::lookupResource(m_managers, id);
}

// RenderCommand updater jobs
QVariant ShaderData::getTransformedProperty(const PropertyValue *v, const Matrix4x4 &viewMatrix) const noexcept
{
    // Note protecting m_worldMatrix at this point as we assume all world updates
    // have been performed when reaching this point
    if (v->isTransformed) {
        const auto transformedIt = m_originalProperties.constFind(v->transformedPropertyName);
        if (transformedIt != m_originalProperties.constEnd()) {
            const PropertyValue &transformedValue = transformedIt.value();
            const TransformType transformType = static_cast<TransformType>(transformedValue.value.toInt());
            switch (transformType) {
            case ModelToEye:
                return QVariant::fromValue(viewMatrix.map(m_worldMatrix.map(Vector3D(v->value.value<QVector3D>()))));
            case ModelToWorld:
                return QVariant::fromValue(m_worldMatrix.map(Vector3D(v->value.value<QVector3D>())));
            case ModelToWorldDirection:
                return QVariant::fromValue(Vector3D(m_worldMatrix * Vector4D(v->value.value<QVector3D>(), 0.0f)));
            case NoTransform:
                break;
            }
        }
    }
    return v->value;
}

// Unit tests only
ShaderData::TransformType ShaderData::propertyTransformType(const QString &name) const
{
    const auto it = m_originalProperties.constFind(name);
    if (it != m_originalProperties.constEnd()) {
        const PropertyValue &propertyValue = it.value();
        if (propertyValue.isTransformed) {
            auto transformedIt = m_originalProperties.constFind(name + QLatin1String("Transformed"));
            if (transformedIt != m_originalProperties.end())
                return static_cast<TransformType>(transformedIt.value().value.toInt());
        }
    }
    return NoTransform;
}

// Called by FramePreparationJob or by RenderView when dealing with lights
void ShaderData::updateWorldTransform(const Matrix4x4 &worldMatrix)
{
    if (m_worldMatrix != worldMatrix) {
        m_worldMatrix = worldMatrix;
    }
}

RenderShaderDataFunctor::RenderShaderDataFunctor(AbstractRenderer *renderer, NodeManagers *managers)
    : m_managers(managers)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderShaderDataFunctor::create(Qt3DCore::QNodeId id) const
{
    ShaderData *backend = m_managers->shaderDataManager()->getOrCreateResource(id);
    backend->setManagers(m_managers);
    backend->setRenderer(m_renderer);
    return backend;
}

Qt3DCore::QBackendNode *RenderShaderDataFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_managers->shaderDataManager()->lookupResource(id);
}

void RenderShaderDataFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_managers->shaderDataManager()->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
