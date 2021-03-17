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
#include <private/qbackendnode_p.h>
#include <private/managers_p.h>
#include <private/nodemanagers_p.h>

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

        const QMetaObject *metaObj = node->metaObject();
        const int propertyOffset = QShaderData::staticMetaObject.propertyOffset();
        const int propertyCount = metaObj->propertyCount();
        // Dynamic properties names
        const auto dynamicPropertyNames = node->dynamicPropertyNames();

        QVector<QString> propertyNames;
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
                    propertyName == QStringLiteral("childNodes")) // We don't handle default Node properties
                continue;

            const QVariant &propertyValue = m_propertyReader->readProperty(node->property(propertyName.toLatin1()));
            bool isNested = false;
            bool isTransformed = false;

            // We check if the property is a QNodeId
            isNested = (propertyValue.userType() == qNodeIdTypeId);
            // We check if QVector<QNodeId>
            if (propertyValue.userType() == QMetaType::QVariantList) {
                QVariantList list = propertyValue.value<QVariantList>();
                if (list.count() > 0 && list.at(0).userType() == qNodeIdTypeId)
                    isNested = true;
            }

            // We check if property is a Transformed property
            if (propertyValue.userType() == QVariant::Vector3D) {
                // if there is a matching QShaderData::TransformType propertyTransformed
                isTransformed = propertyNames.contains(propertyName + QLatin1String("Transformed"));
            }
            m_originalProperties.insert(propertyName, { propertyValue, isNested, isTransformed });
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

ShaderData *ShaderData::lookupResource(NodeManagers *managers, QNodeId id)
{
    return managers->shaderDataManager()->lookupResource(id);
}

ShaderData *ShaderData::lookupResource(QNodeId id)
{
    return ShaderData::lookupResource(m_managers, id);
}

// RenderCommand updater jobs
QVariant ShaderData::getTransformedProperty(const QString &name, const Matrix4x4 &viewMatrix) const noexcept
{
    // Note protecting m_worldMatrix at this point as we assume all world updates
    // have been performed when reaching this point
    const auto it = m_originalProperties.constFind(name);
    if (it != m_originalProperties.constEnd()) {
        const PropertyValue &propertyValue = it.value();
        if (propertyValue.isTransformed) {
            const auto transformedIt = m_originalProperties.constFind(name + QLatin1String("Transformed"));
            if (transformedIt != m_originalProperties.constEnd()) {
                const PropertyValue &transformedValue = transformedIt.value();
                const TransformType transformType = static_cast<TransformType>(transformedValue.value.toInt());
                switch (transformType) {
                case ModelToEye:
                    return QVariant::fromValue(viewMatrix * m_worldMatrix * Vector3D(propertyValue.value.value<QVector3D>()));
                case ModelToWorld:
                    return QVariant::fromValue(m_worldMatrix * Vector3D(propertyValue.value.value<QVector3D>()));
                case ModelToWorldDirection:
                    return QVariant::fromValue(Vector3D(m_worldMatrix * Vector4D(propertyValue.value.value<QVector3D>(), 0.0f)));
                case NoTransform:
                    break;
                }
            }
        }
        return propertyValue.value;
    }
    return QVariant();
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
