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
    , m_needsBufferUpdate(true)
{
}

void RenderShaderData::updateFromPeer(QNode *peer)
{
    m_properties.clear();
    const QShaderData *shaderData = static_cast<const QShaderData *>(peer);
    const QMetaObject *metaObject = shaderData->metaObject();
    const int propertyOffset = metaObject->propertyOffset();
    const int propertyCount = metaObject->propertyCount();

    for (int i = propertyOffset; i < propertyCount; ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (strcmp(property.name(), "data") == 0 || strcmp(property.name(), "childNodes") == 0) // We don't handle default Node properties
            continue;
        m_properties.insert(property.name(), shaderData->property(property.name()));
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
    m_needsBufferUpdate = true;
}

void RenderShaderData::appendActiveProperty(const char *propertyName, const ShaderUniform &description)
{
    m_activeProperties.insert(propertyName, description);
}

void RenderShaderData::updateUniformBuffer(QGraphicsContext *ctx)
{
    if (!m_ubo.isCreated()) {
        m_ubo.create(ctx);
        m_ubo.allocate(ctx, m_block.m_size);
    }

    // TO DO: Only update values that have changed rather than the whole buffer
    QHash<const char *, ShaderUniform>::const_iterator uniformsIt = m_activeProperties.begin();
    const QHash<const char *, ShaderUniform>::const_iterator uniformsEnd = m_activeProperties.end();

    while (uniformsIt != uniformsEnd) {
        ctx->buildUniformBuffer(m_properties.value(uniformsIt.key()), uniformsIt.value(), m_data);
        ++uniformsIt;
    }
    m_ubo.update(ctx, m_data.constData(), m_block.m_size);
}

// Make sure QGraphicsContext::bindUniformBlock is called prior to this
void RenderShaderData::apply(QGraphicsContext *ctx, int bindingPoint)
{
    // Upload new data to GPU if it has changed
    if (m_needsBufferUpdate) {
        updateUniformBuffer(ctx);
        m_needsBufferUpdate = false;
    }
    m_ubo.bindToUniformBlock(ctx, bindingPoint);
}

void RenderShaderData::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (m_properties.contains(propertyChange->propertyName())) {
            m_properties.insert(propertyChange->propertyName(), propertyChange->value());
            m_needsBufferUpdate = true;
        }
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
