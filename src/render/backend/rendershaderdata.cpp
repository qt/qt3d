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

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderShaderData::RenderShaderData()
    : QBackendNode()
    , m_initialized(false)
    , m_mutex(new QMutex)
{
    m_needsBufferUpdate.fetchAndStoreOrdered(1);
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
    if (!m_propertyReader.isNull()) {
        const QMetaObject *metaObject = shaderData->metaObject();
        const int propertyOffset = QShaderData::staticMetaObject.propertyOffset();
        const int propertyCount = metaObject->propertyCount();

        for (int i = propertyOffset; i < propertyCount; ++i) {
            const QMetaProperty property = metaObject->property(i);
            if (strcmp(property.name(), "data") == 0 || strcmp(property.name(), "childNodes") == 0) // We don't handle default Node properties
                continue;
            // We should be making clones of inner QShaderData properties
            // TO DO: If the property is a QShaderData as well, we should register ourself as an observer of updates
            // otherwise we won't ever be notified about a nested property change
            // Q_D(QBackendNode);
            // d->m_arbiter->registerObserver(QBackendNodePrivate::get(this), subQShaderData->nodeId(),NodeUpdated)
            m_properties.insert(QString::fromLatin1(property.name()),
                                m_propertyReader->readProperty(shaderData->property(property.name())));
        }
    }
}

void RenderShaderData::initialize(const ShaderUniformBlock &block)
{
    m_block = block;
    Q_ASSERT(m_block.m_size > 0);
    // Allocate CPU side buffer
    m_data = QByteArray(m_block.m_size, 0);
    m_initialized = true;
    // Force UBO buffer to be allocated on the GPU when apply is called
    m_needsBufferUpdate.fetchAndStoreOrdered(1);
}

void RenderShaderData::appendActiveProperty(const QString &propertyName, const ShaderUniform &description)
{
    m_activeProperties.insert(propertyName, description);
}

// TO DO: This isn't the cleanest solution but for now, this helps us getting UBOs working
// and we'll be able to correct that easily once we have something working
void RenderShaderData::setActiveUniformValues(const QHash<QString, QVariant> &newValues)
{
    QMutexLocker lock(m_mutex);
    m_activeUniformToValues = newValues;
}

void RenderShaderData::updateUniformBuffer(QGraphicsContext *ctx)
{
    const QHash<QString, ShaderUniform>::const_iterator uniformsEnd = m_activeProperties.end();
    QHash<QString, ShaderUniform>::const_iterator uniformsIt = m_activeProperties.begin();

    if (!m_ubo.isCreated()) {
        m_ubo.create(ctx);
        m_ubo.allocate(ctx, m_block.m_size);
        // We need to fill the UBO the first time it is created
        while (uniformsIt != uniformsEnd) {
            if (m_activeUniformToValues.contains(uniformsIt.key()))
                ctx->buildUniformBuffer(m_activeUniformToValues.value(uniformsIt.key()), uniformsIt.value(), m_data);
            ++uniformsIt;
        }
        // Upload the whole buffer to GPU for the first time
        m_ubo.update(ctx, m_data.constData(), m_block.m_size);
    }
    uniformsIt = m_activeProperties.begin();
    while (uniformsIt != uniformsEnd) {
            if (m_activeUniformToValues.contains(uniformsIt.key())) {
                // Update CPU side sub buffer
                ctx->buildUniformBuffer(m_activeUniformToValues.value(uniformsIt.key()), uniformsIt.value(), m_data);
                // Upload sub buffer to GPU
                m_ubo.update(ctx, m_data.constData() + uniformsIt.value().m_offset, uniformsIt.value().m_rawByteSize, uniformsIt.value().m_offset);
        }
        ++uniformsIt;
    }
    m_updatedProperties.clear();
}

// Make sure QGraphicsContext::bindUniformBlock is called prior to this
void RenderShaderData::apply(QGraphicsContext *ctx, int bindingPoint)
{
    // Upload new data to GPU if it has changed
    if (m_needsBufferUpdate.fetchAndStoreOrdered(0)) {
        QMutexLocker lock(m_mutex);
        // We lock in case m_properties is being updated at the same time
        updateUniformBuffer(ctx);
    }
    m_ubo.bindToUniformBlock(ctx, bindingPoint);
}

// We have a concurrency issue here, updateUniformBuffer might be called
// by the RenderThread while sceneChangeEvent is being called by the
// AspectThread / Job thread for transformed properties
void RenderShaderData::sceneChangeEvent(const QSceneChangePtr &e)
{
    // TO DO check if property update comes from the root QShaderData
    // or if it comes from one of the nested QShaderData
    if (!m_propertyReader.isNull() && e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QString propertyName = QString::fromLatin1(propertyChange->propertyName());
        QMutexLocker lock(m_mutex);
        // lock to update m_properties;
        if (m_properties.contains(propertyName)) {
            m_properties.insert(propertyName, m_propertyReader->readProperty(propertyChange->value()));
            m_updatedProperties.append(propertyName);
            m_needsBufferUpdate.fetchAndStoreOrdered(1);
        }
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
