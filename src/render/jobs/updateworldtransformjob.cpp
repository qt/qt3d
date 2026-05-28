// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updateworldtransformjob_p.h"

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qtransform_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/transform_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

struct TransformUpdate
{
    Qt3DCore::QNodeId peerId;
    QMatrix4x4 worldTransformMatrix;
};

namespace {

void updateWorldTransformAndBounds(NodeManagers *manager, Entity *node, const Matrix4x4 &parentTransform, bool hasParentTransform, QList<TransformUpdate> &updatedTransforms)
{
    if (!node->isEnabled())
        return;

    Matrix4x4 worldTransform(parentTransform);
    Transform *nodeTransform = node->renderComponent<Transform>();

    const bool hasTransformComponent = nodeTransform != nullptr && nodeTransform->isEnabled();
    if (hasTransformComponent)
        worldTransform = worldTransform * nodeTransform->transformMatrix();

    if (*(node->worldTransform()) != worldTransform) {
        *(node->worldTransform()) = worldTransform;
        if (hasTransformComponent)
            updatedTransforms.push_back({nodeTransform->peerId(), convertToQMatrix4x4(worldTransform)});
    }
    node->setParentLessTransform(!hasParentTransform);

    const auto &childrenHandles = node->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *child = manager->renderNodesManager()->data(handle);
        if (child)
            updateWorldTransformAndBounds(manager, child, worldTransform, hasParentTransform || hasTransformComponent, updatedTransforms);
    }
}

}

class Q_3DRENDERSHARED_PRIVATE_EXPORT UpdateWorldTransformJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    UpdateWorldTransformJobPrivate() {}
    ~UpdateWorldTransformJobPrivate() override {}

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<TransformUpdate> m_updatedTransforms;
};

UpdateWorldTransformJob::UpdateWorldTransformJob()
    : Qt3DCore::QAspectJob(*new UpdateWorldTransformJobPrivate())
    , m_node(nullptr)
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateTransform, 0)
}

void UpdateWorldTransformJob::setRoot(Entity *root)
{
    m_node = root;
}

void UpdateWorldTransformJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void UpdateWorldTransformJob::run()
{
    // Iterate over each level of hierarchy in our scene
    // and update each node's world transform from its
    // local transform and its parent's world transform

    // TODO: Parallelise this on each level using a parallel_for
    // implementation.

    Q_D(UpdateWorldTransformJob);
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();

    Matrix4x4 parentTransform;
    Entity *parent = m_node->parent();
    if (parent != nullptr)
        parentTransform = *(parent->worldTransform());
    updateWorldTransformAndBounds(m_manager, m_node, parentTransform, false, d->m_updatedTransforms);

    qCDebug(Jobs) << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

void UpdateWorldTransformJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    const QList<TransformUpdate> updatedTransforms = Qt3DCore::moveAndClear(m_updatedTransforms);
    for (const TransformUpdate &t : updatedTransforms) {
        Qt3DCore::QTransform *node =
                qobject_cast<Qt3DCore::QTransform *>(manager->lookupNode(t.peerId));
        if (!node)
            continue;
        Qt3DCore::QTransformPrivate *dNode =
                static_cast<Qt3DCore::QTransformPrivate *>(Qt3DCore::QNodePrivate::get(node));
        dNode->setWorldMatrix(t.worldTransformMatrix);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
