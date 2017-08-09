/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "joint_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DCore/private/qjoint_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Joint::Joint()
    : BackendNode(Qt3DCore::QBackendNode::ReadOnly)
    , m_localPose()
    , m_jointManager(nullptr)
{
}

void Joint::cleanup()
{
    m_inverseBindMatrix.setToIdentity();
    m_localPose = Sqt();
    m_childJointIds.clear();
    m_name.clear();
    setEnabled(false);
}

void Joint::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    Q_ASSERT(m_jointManager);
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QJointData>>(change);
    const auto &data = typedChange->data;
    m_inverseBindMatrix = data.inverseBindMatrix;
    m_localPose.rotation = data.rotation;
    m_localPose.scale = data.scale;
    m_localPose.translation = data.translation;
    m_childJointIds = data.childJointIds;
    markDirty(AbstractRenderer::JointDirty);
    m_jointManager->addDirtyJoint(peerId());

    // TODO: Add name property to QJoint
}

void Joint::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == PropertyUpdated) {
        const QPropertyUpdatedChangePtr &propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("scale")) {
            m_localPose.scale = propertyChange->value().value<QVector3D>();
            markDirty(AbstractRenderer::JointDirty);
            m_jointManager->addDirtyJoint(peerId());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("rotation")) {
            m_localPose.rotation = propertyChange->value().value<QQuaternion>();
            markDirty(AbstractRenderer::JointDirty);
            m_jointManager->addDirtyJoint(peerId());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("translation")) {
            m_localPose.translation = propertyChange->value().value<QVector3D>();
            markDirty(AbstractRenderer::JointDirty);
            m_jointManager->addDirtyJoint(peerId());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("inverseBindMatrix")) {
            m_inverseBindMatrix = propertyChange->value().value<QMatrix4x4>();
            markDirty(AbstractRenderer::JointDirty);
            m_jointManager->addDirtyJoint(peerId());

            // TODO: Mark just the inverse bind matrix as dirty
        }
        // TODO: Add name property to QJoint
    } else if (e->type() == PropertyValueAdded) {
        const auto addedChange = qSharedPointerCast<QPropertyNodeAddedChange>(e);
        if (addedChange->propertyName() == QByteArrayLiteral("childJoint"))
            m_childJointIds.push_back(addedChange->addedNodeId());
    } else if (e->type() == PropertyValueRemoved) {
        const auto removedChange = qSharedPointerCast<QPropertyNodeRemovedChange>(e);
        if (removedChange->propertyName() == QByteArrayLiteral("childJoint"))
            m_childJointIds.removeOne(removedChange->removedNodeId());
    }

    BackendNode::sceneChangeEvent(e);
}


JointFunctor::JointFunctor(AbstractRenderer *renderer, JointManager *jointManager)
    : m_renderer(renderer)
    , m_jointManager(jointManager)
{
}

Qt3DCore::QBackendNode *JointFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Joint *backend = m_jointManager->getOrCreateResource(change->subjectId());
    backend->setRenderer(m_renderer);
    backend->setJointManager(m_jointManager);
    return backend;
}

Qt3DCore::QBackendNode *JointFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_jointManager->lookupResource(id);
}

void JointFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_jointManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
