// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LOADSKELETONJOB_P_H
#define QT3DRENDER_RENDER_LOADSKELETONJOB_P_H

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

#include <Qt3DCore/qaspectjob.h>

#include <QtCore/qsharedpointer.h>
#include <Qt3DRender/private/skeletondata_p.h>
#include <Qt3DRender/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QJoint;
}

namespace Qt3DRender {
namespace Render {

class NodeManagers;
class LoadSkeletonJobPrivate;

class LoadSkeletonJob : public Qt3DCore::QAspectJob
{
public:
    explicit LoadSkeletonJob(const HSkeleton &handle);

    void setNodeManagers(NodeManagers *nodeManagers) { m_nodeManagers = nodeManagers; }

protected:
    void run() override;
    void loadSkeleton(Skeleton *skeleton);
    void loadSkeletonFromUrl(Skeleton *skeleton);
    void loadSkeletonFromData(Skeleton *skeleton);
    Qt3DCore::QJoint *createFrontendJoints(const SkeletonData &skeletonData) const;
    Qt3DCore::QJoint *createFrontendJoint(const QString &jointName,
                                          const Qt3DCore::Sqt &localPose,
                                          const QMatrix4x4 &inverseBindMatrix) const;
    void processJointHierarchy(Qt3DCore::QNodeId jointId,
                               int parentJointIndex,
                               SkeletonData &skeletonData);

    HSkeleton m_handle;
    NodeManagers *m_nodeManagers;

private:
    Q_DECLARE_PRIVATE(LoadSkeletonJob)
};

typedef QSharedPointer<LoadSkeletonJob> LoadSkeletonJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LOADSKELETONJOB_P_H
