/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "renderrenderpass_p.h"
#include <Qt3DRenderer/private/renderannotation_p.h>
#include <Qt3DRenderer/qparametermapping.h>
#include <Qt3DRenderer/qrenderstate.h>
#include <Qt3DRenderer/qrenderpass.h>

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

    if (pass->shaderProgram() != Q_NULLPTR)
        m_shaderUuid = pass->shaderProgram()->id();
    // The RenderPass clones frontend bindings in case the frontend ever removes them
    // TO DO: We probably need a QParameterMapper manager
    Q_FOREACH (QParameterMapping *binding, pass->bindings())
        appendBinding(binding);
    Q_FOREACH (QAnnotation *c, pass->annotations())
        appendAnnotation(c->id());
    Q_FOREACH (QRenderState *renderState, pass->renderStates())
        appendRenderState(renderState);
}

void RenderRenderPass::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            appendAnnotation(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("shaderProgram")) {
            m_shaderUuid = propertyChange->value().value<QNodeId>();
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("binding")) {
            appendBinding(propertyChange->value().value<QParameterMapping *>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("renderState")) {
            appendRenderState(propertyChange->value().value<QRenderState *>());
        }
        break;
    }

    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            removeAnnotation(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("shaderProgram")) {
            m_shaderUuid = QNodeId();
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("binding")) {
            removeBinding(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("renderState")) {
            removeRenderState(propertyChange->value().value<QNodeId>());
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

QList<QParameterMapping *> RenderRenderPass::bindings() const
{
    return m_bindings.values();
}

QList<QNodeId> RenderRenderPass::annotations() const
{
    return m_annotationList;
}

QList<QRenderState *> RenderRenderPass::renderStates() const
{
    return m_renderStates.values();
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

void RenderRenderPass::appendBinding(QParameterMapping *binding)
{
    if (!m_bindings.contains(binding->id()))
        m_bindings[binding->id()] = binding;
}

void RenderRenderPass::removeBinding(const QNodeId &bindingId)
{
    m_bindings.remove(bindingId);
}

void RenderRenderPass::appendRenderState(QRenderState *renderState)
{
    if (!m_renderStates.contains(renderState->id()))
        m_renderStates[renderState->id()] = renderState;
}

void RenderRenderPass::removeRenderState(const QNodeId &renderStateId)
{
    m_renderStates.remove(renderStateId);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
