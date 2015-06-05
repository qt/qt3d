/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "statesetnode_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qstateset.h>
#include <Qt3DRenderer/private/renderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

StateSetNode::StateSetNode()
    : FrameGraphNode(FrameGraphNode::StateSet)
{
}

StateSetNode::~StateSetNode()
{
}

void StateSetNode::updateFromPeer(QNode *peer)
{
    QStateSet *stateSet = static_cast<QStateSet*>(peer);

    setEnabled(stateSet->isEnabled());
    Q_FOREACH (QRenderState *renderState, stateSet->renderStates())
        appendRenderState(renderState->id(), RenderState::getOrCreateBackendState(renderState));
}

QList<RenderState *> StateSetNode::renderStates() const
{
    return m_renderStates.values();
}

void StateSetNode::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {
    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("renderState")) {
            QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
            QRenderState *renderState = static_cast<QRenderState *>(nodePtr.data());
            appendRenderState(renderState->id(), RenderState::getOrCreateBackendState(renderState));
        }
    }
        break;

    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("renderState"))
            removeRenderState(propertyChange->value().value<QNodeId>());
    }
        break;

    default:
        break;
    }
}

void StateSetNode::appendRenderState(const QNodeId &id, RenderState *renderState)
{
    if (!m_renderStates.contains(id))
        m_renderStates[id] = renderState;
}

void StateSetNode::removeRenderState(const QNodeId &renderStateId)
{
    m_renderStates.remove(renderStateId);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
