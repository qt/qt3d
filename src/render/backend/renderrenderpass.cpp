/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderrenderpass_p.h"
#include <Qt3DRenderer/private/renderannotation_p.h>
#include <Qt3DRenderer/qparametermapping.h>
#include <Qt3DRenderer/qrenderstate.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qparameter.h>

// TODO: Rename this include to something more descriptive
#include <Qt3DRenderer/private/blendstate_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderRenderPass::RenderRenderPass()
    : QBackendNode()
{
}

RenderRenderPass::~RenderRenderPass()
{
    cleanup();
}

void RenderRenderPass::cleanup()
{
}

void RenderRenderPass::updateFromPeer(QNode *peer)
{
    QRenderPass *pass = static_cast<QRenderPass *>(peer);

    m_parameterPack.clear();

    if (pass->shaderProgram() != Q_NULLPTR)
        m_shaderUuid = pass->shaderProgram()->id();
    // The RenderPass clones frontend bindings in case the frontend ever removes them
    // TO DO: We probably need a QParameterMapper manager
    Q_FOREACH (QParameterMapping *binding, pass->bindings())
        appendBinding(RenderParameterMapping(binding));
    Q_FOREACH (QAnnotation *c, pass->annotations())
        appendAnnotation(c->id());
    Q_FOREACH (QRenderState *renderState, pass->renderStates())
        appendRenderState(renderState->id(), RenderState::getOrCreateBackendState(renderState));
    Q_FOREACH (QParameter *p, pass->parameters())
        m_parameterPack.appendParameter(p->id());
}

void RenderRenderPass::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            appendAnnotation(propertyChange->value().value<QNodeId>());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("shaderProgram")) {
            m_shaderUuid = propertyChange->value().value<QNodeId>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("binding")) {
            appendBinding(RenderParameterMapping(propertyChange->value().value<QParameterMapping *>()));
        } else if (propertyChange->propertyName() == QByteArrayLiteral("renderState")) {
            QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
            QRenderState *renderState = static_cast<QRenderState *>(nodePtr.data());
            appendRenderState(renderState->id(), RenderState::getOrCreateBackendState(renderState));
        } else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.appendParameter(propertyChange->value().value<QNodeId>());
        }
        break;
    }

    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            removeAnnotation(propertyChange->value().value<QNodeId>());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("shaderProgram")) {
            m_shaderUuid = QNodeId();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("binding")) {
            removeBinding(propertyChange->value().value<QNodeId>());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("renderState")) {
            removeRenderState(propertyChange->value().value<QNodeId>());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.removeParameter(propertyChange->value().value<QNodeId>());
        }
        break;
    }

    default:
        break;
    }
}

QNodeId RenderRenderPass::shaderProgram() const
{
    return m_shaderUuid;
}

QList<RenderParameterMapping> RenderRenderPass::bindings() const
{
    return m_bindings.values();
}

QList<QNodeId> RenderRenderPass::annotations() const
{
    return m_annotationList;
}

QList<RenderState *> RenderRenderPass::renderStates() const
{
    return m_renderStates.values();
}

QList<QNodeId> RenderRenderPass::parameters() const
{
    return m_parameterPack.parameters();
}

void RenderRenderPass::appendAnnotation(const QNodeId &annotationId)
{
    if (!m_annotationList.contains(annotationId))
        m_annotationList.append(annotationId);
}

void RenderRenderPass::removeAnnotation(const QNodeId &annotationId)
{
    m_annotationList.removeOne(annotationId);
}

void RenderRenderPass::appendBinding(const RenderParameterMapping &binding)
{
    if (!m_bindings.contains(binding.id()))
        m_bindings[binding.id()] = binding;
}

void RenderRenderPass::removeBinding(const QNodeId &bindingId)
{
    m_bindings.remove(bindingId);
}

void RenderRenderPass::appendRenderState(const QNodeId &id, RenderState *renderState)
{
    if (!m_renderStates.contains(id))
        m_renderStates[id] = renderState;
}

void RenderRenderPass::removeRenderState(const QNodeId &renderStateId)
{
    m_renderStates.remove(renderStateId);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
