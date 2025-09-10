// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_SKELETON_H
#define QT3DRENDER_RENDER_SKELETON_H

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
#include <Qt3DRender/private/skeletondata_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DCore/qskeletonloader.h>

#include <QtGui/qmatrix4x4.h>
#include <QDebug>

#if defined(QT_BUILD_INTERNAL)
class tst_Skeleton;
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QJoint;
}

namespace Qt3DRender {
namespace Render {

class JointManager;
class SkeletonManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Skeleton : public BackendNode
{
public:
    enum SkeletonDataType {
        Unknown,
        File,
        Data
    };

    Skeleton();

    void setSkeletonManager(SkeletonManager *skeletonManager) { m_skeletonManager = skeletonManager; }
    SkeletonManager *skeletonManager() const { return m_skeletonManager; }

    void setJointManager(JointManager *jointManager) { m_jointManager = jointManager; }
    JointManager *jointManager() const { return m_jointManager; }

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void setStatus(Qt3DCore::QSkeletonLoader::Status status);
    Qt3DCore::QSkeletonLoader::Status status() const { return m_status; }

    QUrl source() const { return m_source; }
    SkeletonDataType dataType() const { return m_dataType; }
    bool createJoints() const { return m_createJoints; }

    void setName(const QString &name) { m_name = name; }
    QString name() const { return m_name; }

    qsizetype jointCount() const { return m_skeletonData.joints.size(); }
    QVector<JointInfo> joints() const { return m_skeletonData.joints; }
    QVector<QString> jointNames() const { return m_skeletonData.jointNames; }
    QVector<Qt3DCore::Sqt> localPoses() const { return m_skeletonData.localPoses; }

    Qt3DCore::QNodeId rootJointId() const { return m_rootJointId; }

    // Called from jobs
    void setLocalPose(HJoint jointHandle, const Qt3DCore::Sqt &localPose);
    QVector<QMatrix4x4> calculateSkinningMatrixPalette();

    void clearData();
    void setSkeletonData(const SkeletonData &data);
    const SkeletonData &skeletonData() const { return m_skeletonData; }
    SkeletonData skeletonData() { return m_skeletonData; }

    // Allow unit tests to set the data type
#if defined(QT_BUILD_INTERNAL)
public:
    void setDataType(SkeletonDataType dataType) { m_dataType = dataType; }
#endif

private:
    QVector<QMatrix4x4> m_skinningPalette;

    // QSkeletonLoader Properties
    QUrl m_source;
    Qt3DCore::QSkeletonLoader::Status m_status;
    bool m_createJoints;

    // QSkeleton properties
    Qt3DCore::QNodeId m_rootJointId;

    SkeletonDataType m_dataType;

    QString m_name;
    SkeletonData m_skeletonData;
    SkeletonManager *m_skeletonManager;
    JointManager *m_jointManager;
    HSkeleton m_skeletonHandle; // Our own handle to set on joints

#if defined(QT_BUILD_INTERNAL)
    friend class ::tst_Skeleton;
#endif
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const Skeleton &skeleton)
{
    QDebugStateSaver saver(dbg);
    dbg << "QNodeId =" << skeleton.peerId() << Qt::endl
        << "Name =" << skeleton.name() << Qt::endl;
    return dbg;
}
#endif

class SkeletonFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit SkeletonFunctor(AbstractRenderer *renderer,
                             SkeletonManager *skeletonManager,
                             JointManager *jointManager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    AbstractRenderer *m_renderer;
    SkeletonManager *m_skeletonManager;
    JointManager *m_jointManager;
};

} // namespace Render
} // namespace Qt3DRender


QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SKELETON_H
