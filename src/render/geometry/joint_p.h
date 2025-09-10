// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_JOINT_H
#define QT3DRENDER_RENDER_JOINT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DCore/private/sqt_p.h>
#include <Qt3DRender/private/handle_types_p.h>

#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class JointManager;
class SkeletonManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Joint : public BackendNode
{
public:
    Joint();

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::Sqt localPose() const { return m_localPose; }
    QMatrix4x4 inverseBindMatrix() const { return m_inverseBindMatrix; }
    QString name() const { return m_name; }
    QList<Qt3DCore::QNodeId> childJointIds() const { return m_childJointIds; }

    QVector3D translation() const { return m_localPose.translation; }
    QQuaternion rotation() const { return m_localPose.rotation; }
    QVector3D scale() const { return m_localPose.scale; }

    void setOwningSkeleton(HSkeleton skeletonHandle) { m_owningSkeleton = skeletonHandle; }
    HSkeleton owningSkeleton() const { return m_owningSkeleton; }

    void setJointManager(JointManager *jointManager) { m_jointManager = jointManager; }
    JointManager *jointManager() const { return m_jointManager; }

    void setSkeletonManager(SkeletonManager *skeletonManager) { m_skeletonManager = skeletonManager; }
    SkeletonManager *skeletonManager() const { return m_skeletonManager; }

private:
    QMatrix4x4 m_inverseBindMatrix;
    Qt3DCore::Sqt m_localPose;
    QList<Qt3DCore::QNodeId> m_childJointIds;
    QString m_name;
    JointManager *m_jointManager;
    SkeletonManager *m_skeletonManager;
    HSkeleton m_owningSkeleton;
};

class JointFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit JointFunctor(AbstractRenderer *renderer,
                          JointManager *jointManager,
                          SkeletonManager *skeletonManager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    AbstractRenderer *m_renderer;
    JointManager *m_jointManager;
    SkeletonManager *m_skeletonManager;
};

} // namespace Render
} // namespace Qt3DRender


QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_JOINT_H
