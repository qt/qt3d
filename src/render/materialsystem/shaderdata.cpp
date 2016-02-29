/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "shaderdata_p.h"
#include "qshaderdata.h"
#include "qshaderdata_p.h"
#include <QMetaProperty>
#include <QMetaObject>
#include <Qt3DCore/qdynamicpropertyupdatedchange.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <private/graphicscontext_p.h>
#include <private/qbackendnode_p.h>
#include <private/glbuffer_p.h>
#include <private/managers_p.h>
#include <private/nodemanagers_p.h>
#include <private/renderviewjobutils_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

namespace {

const int qNodeIdTypeId = qMetaTypeId<Qt3DCore::QNodeId>();

}

QVector<Qt3DCore::QNodeId> ShaderData::m_updatedShaderData;

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

void ShaderData::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QShaderDataData>>(change);
    const QShaderDataData &data = typedChange->data;

    m_propertyReader = data.propertyReader;

    for (const QPair<QByteArray, QVariant> &entry : data.properties) {
        if (entry.first == QByteArrayLiteral("data") ||
                entry.first == QByteArrayLiteral("childNodes")) // We don't handle default Node properties
            continue;
        const QVariant &propertyValue = entry.second;
        const QString propertyName = QString::fromLatin1(entry.first);

        m_properties.insert(propertyName, propertyValue);
        m_originalProperties.insert(propertyName, propertyValue);

        // We check if the property is a QNodeId or QVector<QNodeId> so that we can
        // check nested QShaderData for update
        if (propertyValue.userType() == qNodeIdTypeId) {
            m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        } else if (propertyValue.userType() == QMetaType::QVariantList) {
            QVariantList list = propertyValue.value<QVariantList>();
            if (list.count() > 0 && list.at(0).userType() == qNodeIdTypeId)
                m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        }
    }
    // Note: we ignore transformed properties for now until we figure out how to best handle them
}


ShaderData *ShaderData::lookupResource(NodeManagers *managers, QNodeId id)
{
    return managers->shaderDataManager()->lookupResource(id);
}

ShaderData *ShaderData::lookupResource(QNodeId id)
{
    return ShaderData::lookupResource(m_managers, id);
}

// Call by cleanup job (single thread)
void ShaderData::clearUpdatedProperties()
{
    m_updatedProperties.clear();
    const QHash<QString, QVariant>::const_iterator end = m_nestedShaderDataProperties.end();
    QHash<QString, QVariant>::const_iterator it = m_nestedShaderDataProperties.begin();

    while (it != end) {
        if (it.value().userType() == QMetaType::QVariantList) {
            const auto values = it.value().value<QVariantList>();
            for (const QVariant &v : values) {
                ShaderData *nested = lookupResource(v.value<QNodeId>());
                if (nested != nullptr)
                    nested->clearUpdatedProperties();
            }
        } else {
            ShaderData *nested = lookupResource(it.value().value<QNodeId>());
            if (nested != nullptr)
                nested->clearUpdatedProperties();
        }
        ++it;
    }
}

void ShaderData::cleanup(NodeManagers *managers)
{
    for (Qt3DCore::QNodeId id : qAsConst(m_updatedShaderData)) {
        ShaderData *shaderData = ShaderData::lookupResource(managers, id);
        if (shaderData)
            shaderData->clearUpdatedProperties();
    }
    m_updatedShaderData.clear();
}

// Called by renderview jobs (several concurrent threads)
/*!
  \internal
  Lookup if the current ShaderData or a nested ShaderData has updated properties.
  UpdateProperties contains either the value of the propertie of a QNodeId if it's another ShaderData.
  Transformed properties are updated for all of ShaderData that have ones at the point.

  \note This needs to be performed for every top level ShaderData every time it is used.
  As we don't know if the transformed properties use the same viewMatrix for all RenderViews.
 */
bool ShaderData::updateViewTransform(const QMatrix4x4 &viewMatrix)
{
    // We can't perform this only once as we don't know if we would be call as the root or a
    // nested ShaderData
    QMutexLocker lock(&m_mutex);

    // Update transformed properties
    // We check the matrices and decide if the transform has changed since the previous call to needsUpdate
    if (m_viewMatrix != viewMatrix) {
        m_viewMatrix = viewMatrix;
        const QHash<QString, TransformType>::const_iterator transformedEnd = m_transformedProperties.end();
        QHash<QString, TransformType>::const_iterator transformedIt = m_transformedProperties.begin();

        while (transformedIt != transformedEnd) {
            if (transformedIt.value() == ModelToEye) {
                m_updatedProperties.insert(transformedIt.key(), m_viewMatrix * m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
                m_properties.insert(transformedIt.key(), m_viewMatrix * m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
            }
            ++transformedIt;
        }
    }
    const QHash<QString, QVariant>::const_iterator end = m_nestedShaderDataProperties.end();
    QHash<QString, QVariant>::const_iterator it = m_nestedShaderDataProperties.begin();

    while (it != end) {
        const int userType = it.value().userType();

        if (userType == QMetaType::QVariantList) {
            QVariantList updatedNodes;
            bool nestedNeedsUpdate = false;
            const QVariantList values = variant_value<QVariantList>(it.value());
            for (const QVariant &v : values) {
                if (v.userType() != qNodeIdTypeId)
                    continue;

                const Qt3DCore::QNodeId nestedId = variant_value<Qt3DCore::QNodeId>(v);
                ShaderData *nested = lookupResource(nestedId);
                if (nested != nullptr) {
                    // We need to add the nested nodes to the updated property list
                    // as we need to maintain order
                    // if node[0] doesn't need update but node[1] does,
                    // if we only have a single element, the renderer would update element [0]
                    nestedNeedsUpdate |= nested->updateViewTransform(viewMatrix);
                    updatedNodes << v;
                }
            }
            // Of course we only add all the nodes if at least one of the nested nodes required and update
            if (nestedNeedsUpdate && !updatedNodes.empty())
                m_updatedProperties.insert(it.key(), updatedNodes);
        } else if (userType == qNodeIdTypeId) {
            const Qt3DCore::QNodeId nestedId = variant_value<Qt3DCore::QNodeId>(it.value());
            ShaderData *nested = lookupResource(nestedId);
            if (nested != nullptr && nested->updateViewTransform(viewMatrix))
                m_updatedProperties.insert(it.key(), it.value());
        }
        ++it;
    }
    return m_updatedProperties.size() > 0;
}

bool ShaderData::updateWorldTransform(const QMatrix4x4 &worldMatrix)
{
    if (m_worldMatrix != worldMatrix) {
        m_worldMatrix = worldMatrix;

        const QHash<QString, TransformType>::const_iterator transformedEnd = m_transformedProperties.end();
        QHash<QString, TransformType>::const_iterator transformedIt = m_transformedProperties.begin();

        while (transformedIt != transformedEnd) {
            if (transformedIt.value() == ModelToEye) {
                m_updatedProperties.insert(transformedIt.key(), m_viewMatrix * m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
                m_properties.insert(transformedIt.key(), m_viewMatrix * m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
            } else {
                m_updatedProperties.insert(transformedIt.key(), m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
                m_properties.insert(transformedIt.key(), m_worldMatrix * m_originalProperties.value(transformedIt.key()).value<QVector3D>());
            }
            ++transformedIt;
        }
    }
    return m_updatedProperties.size() > 0;
}

// This will add the ShaderData to be cleared from updates at the end of the frame
// by the cleanup job
// Called by renderview jobs (several concurrent threads)
void ShaderData::markDirty()
{
    QMutexLocker lock(&m_mutex);
    if (!ShaderData::m_updatedShaderData.contains(peerId()))
        ShaderData::m_updatedShaderData.append(peerId());
}

void ShaderData::readPeerProperties(QShaderData *shaderData)
{
    const QMetaObject *metaObject = shaderData->metaObject();
    const int propertyOffset = QShaderData::staticMetaObject.propertyOffset();
    const int propertyCount = metaObject->propertyCount();

    for (int i = propertyOffset; i < propertyCount; ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (strcmp(property.name(), "data") == 0 || strcmp(property.name(), "childNodes") == 0) // We don't handle default Node properties
            continue;
        QVariant propertyValue = m_propertyReader->readProperty(shaderData->property(property.name()));
        QString propertyName = QString::fromLatin1(property.name());

        m_properties.insert(propertyName, propertyValue);
        m_originalProperties.insert(propertyName, propertyValue);

        // We check if the property is a QNodeId or QVector<QNodeId> so that we can
        // check nested QShaderData for update
        if (propertyValue.userType() == qNodeIdTypeId) {
            m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        } else if (propertyValue.userType() == QMetaType::QVariantList) {
            QVariantList list = propertyValue.value<QVariantList>();
            if (list.count() > 0 && list.at(0).userType() == qNodeIdTypeId)
                m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        }
    }

    // Also check the dynamic properties
    const auto propertyNames = shaderData->dynamicPropertyNames();
    for (const QByteArray &propertyName : propertyNames) {
        if (propertyName == "data" || propertyName == "childNodes")  // We don't handle default Node properties
            continue;

        QVariant value = m_propertyReader->readProperty(shaderData->property(propertyName));
        QString key = QString::fromLatin1(propertyName);

        m_properties.insert(key, value);
        m_originalProperties.insert(key, value);
    }


    // We look for transformed properties
    QHash<QString, QVariant>::iterator it = m_properties.begin();
    const QHash<QString, QVariant>::iterator end = m_properties.end();

    while (it != end) {
        if (static_cast<QMetaType::Type>(it.value().type()) == QMetaType::QVector3D) {
            // if there is a matching QShaderData::TransformType propertyTransformed
            QVariant value = m_properties.value(it.key() + QLatin1String("Transformed"));
            // if that's the case, we apply a space transformation to the property
            if (value.isValid() && value.type() == QVariant::Int)
                m_transformedProperties.insert(it.key(), static_cast<TransformType>(value.toInt()));
        }
        ++it;
    }
}

void ShaderData::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (!m_propertyReader.isNull() && e->type() == PropertyUpdated) {
        QString propertyName;
        QVariant propertyValue;

        if (auto propertyChange = qSharedPointerDynamicCast<QPropertyUpdatedChange>(e)) {
            propertyName = QString::fromLatin1(propertyChange->propertyName());
            propertyValue =  m_propertyReader->readProperty(propertyChange->value());
        } else if (auto propertyChange = qSharedPointerDynamicCast<QDynamicPropertyUpdatedChange>(e)) {
            propertyName = QString::fromLatin1(propertyChange->propertyName());
            propertyValue = m_propertyReader->readProperty(propertyChange->value());
        } else {
            Q_UNREACHABLE();
        }

        // Note we aren't notified about nested QShaderData in this call
        // only scalar / vec properties
        if (m_properties.contains(propertyName)) {
            // If this is a Transformed property, we need to multiply against the correct
            // matrices
            m_originalProperties.insert(propertyName, propertyValue);
            if (m_transformedProperties.contains(propertyName)) {
                if (m_transformedProperties[propertyName] == ModelToEye)
                    propertyValue = m_viewMatrix * m_worldMatrix * propertyValue.value<QVector3D>();
                else
                    propertyValue = m_worldMatrix * propertyValue.value<QVector3D>();
            }
            m_properties.insert(propertyName, propertyValue);
            m_updatedProperties.insert(propertyName, propertyValue);
        }
        BackendNode::markDirty(AbstractRenderer::AllDirty);
    }

    BackendNode::sceneChangeEvent(e);
}

RenderShaderDataFunctor::RenderShaderDataFunctor(AbstractRenderer *renderer, NodeManagers *managers)
    : m_managers(managers)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderShaderDataFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    ShaderData *backend = m_managers->shaderDataManager()->getOrCreateResource(change->subjectId());
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
