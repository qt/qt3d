/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rendershaderdata_p.h"
#include "qshaderdata.h"
#include "qshaderdata_p.h"
#include <QMetaProperty>
#include <QMetaObject>
#include <Qt3DCore/qscenepropertychange.h>
#include <private/qgraphicscontext_p.h>
#include <private/qbackendnode_p.h>
#include <private/uniformbuffer_p.h>
#include <private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

QList<QNodeId> RenderShaderData::m_updatedShaderData;

RenderShaderData::RenderShaderData()
    : QBackendNode()
    , m_manager(Q_NULLPTR)
    , m_mutex(new QMutex())
{
}

RenderShaderData::~RenderShaderData()
{
    delete m_mutex;
}

void RenderShaderData::updateFromPeer(QNode *peer)
{
    m_properties.clear();
    const QShaderData *shaderData = static_cast<const QShaderData *>(peer);
    m_propertyReader = shaderData->propertyReader();
    if (!m_propertyReader.isNull())
        readPeerProperties(const_cast<QShaderData *>(shaderData));
}

// Call by cleanup job (single thread)
void RenderShaderData::clearUpdate()
{
    m_updatedProperties.clear();
    const QHash<QString, QVariant>::const_iterator end = m_nestedShaderDataProperties.end();
    QHash<QString, QVariant>::const_iterator it = m_nestedShaderDataProperties.begin();

    while (it != end) {
        if (it.value().userType() == QMetaType::QVariantList) {
            Q_FOREACH (const QVariant &v, it.value().value<QVariantList>()) {
                RenderShaderData *nested = m_manager->lookupResource(v.value<QNodeId>());
                if (nested != Q_NULLPTR)
                    nested->clearUpdate();
            }
        } else {
            RenderShaderData *nested = m_manager->lookupResource(it.value().value<QNodeId>());
            if (nested != Q_NULLPTR)
                nested->clearUpdate();
        }
        ++it;
    }
}

// Called by renderview jobs (several concurrent threads)
bool RenderShaderData::needsUpdate()
{
    // We can't perform this only once as we don't know if we would be call as the root or a
    // nested RenderShaderData
    QMutexLocker lock(m_mutex);
    const QHash<QString, QVariant>::const_iterator end = m_nestedShaderDataProperties.end();
    QHash<QString, QVariant>::const_iterator it = m_nestedShaderDataProperties.begin();

    while (it != end) {
        if (it.value().userType() == QMetaType::QVariantList) {
            QVariantList updatedNodes;
            Q_FOREACH (const QVariant &v, it.value().value<QVariantList>()) {
                RenderShaderData *nested = m_manager->lookupResource(v.value<QNodeId>());
                if (nested != Q_NULLPTR && nested->needsUpdate())
                    updatedNodes << v;
            }
            if (!updatedNodes.empty())
                m_updatedProperties.insert(it.key(), updatedNodes);
        } else {
            RenderShaderData *nested = m_manager->lookupResource(it.value().value<QNodeId>());
            if (nested != Q_NULLPTR && nested->needsUpdate())
                m_updatedProperties.insert(it.key(), it.value());
        }
        ++it;
    }
    return m_updatedProperties.size() > 0;
}

// This will add the RenderShaderData to be cleared from updates at the end of the frame
// by the cleanup job
// Called by renderview jobs (several concurrent threads)
void RenderShaderData::addToClearUpdateList()
{
    QMutexLocker lock(m_mutex);
    if (!RenderShaderData::m_updatedShaderData.contains(peerUuid()))
        RenderShaderData::m_updatedShaderData.append(peerUuid());
}

const int qNodeIdTypeId = qMetaTypeId<QNodeId>();

void RenderShaderData::readPeerProperties(QShaderData *shaderData)
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

        // We check if the property is a QNodeId or QList<QNodeId> so that we can
        // check nested QShaderData for update
        if (propertyValue.userType() == qNodeIdTypeId) {
            m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        } else if (propertyValue.userType() == QMetaType::QVariantList) {
            QVariantList list = propertyValue.value<QVariantList>();
            if (list.count() > 0 && list.at(0).userType() == qNodeIdTypeId)
                m_nestedShaderDataProperties.insert(propertyName, propertyValue);
        }
    }
}

void RenderShaderData::setManager(ShaderDataManager *manager)
{
    m_manager = manager;
}

void RenderShaderData::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (!m_propertyReader.isNull()) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QString propertyName = QString::fromLatin1(propertyChange->propertyName());
        switch (e->type()) {
        case NodeUpdated: {
            // Note we aren't notified about nested QShaderData in this call
            // only scalar / vec properties
            if (m_properties.contains(propertyName)) {
                const QVariant &val = m_propertyReader->readProperty(propertyChange->value());
                m_properties.insert(propertyName, val);
                m_updatedProperties.insert(propertyName, val);
            }
            break;
        }
        case NodeAdded: {
            m_properties.insert(propertyName, m_propertyReader->readProperty(propertyChange->value()));
            m_nestedShaderDataProperties.insert(propertyName, propertyChange->value());
            break;
        }
        case NodeRemoved: {
            if (m_properties.contains(propertyName)) {
                m_properties.remove(propertyName);
                m_nestedShaderDataProperties.remove(propertyName);
            }
            break;
        }
        default:
            break;
        }
    }
}

RenderShaderDataFunctor::RenderShaderDataFunctor(ShaderDataManager *manager)
    : m_manager(manager)
{
}

QBackendNode *RenderShaderDataFunctor::create(QNode *frontend) const
{
    RenderShaderData *backend = m_manager->getOrCreateResource(frontend->id());
    backend->setManager(m_manager);
    backend->setPeer(frontend);
    return backend;
}

QBackendNode *RenderShaderDataFunctor::get(QNode *frontend) const
{
    return m_manager->lookupResource(frontend->id());
}

void RenderShaderDataFunctor::destroy(QNode *frontend) const
{
    m_manager->releaseResource(frontend->id());
}

} // Render

} // Qt3D

QT_END_NAMESPACE
