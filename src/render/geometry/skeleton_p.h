/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

class SkeletonManager;

class Q_AUTOTEST_EXPORT Skeleton : public BackendNode
{
public:
    Skeleton();

    void cleanup();
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void setStatus(Qt3DCore::QSkeletonLoader::Status status);
    Qt3DCore::QSkeletonLoader::Status status() const { return m_status; }

    QUrl source() const { return m_source; }

    void setName(const QString &name) { m_name = name; }
    QString name() const { return m_name; }

    int jointCount() const { return m_skeletonData.joints.size(); }
    void notifyJointCount();
    QVector<JointInfo> joints() const { return m_skeletonData.joints; }

    Qt3DCore::QNodeId rootJointId() const { return m_rootJointId; }

    // Called from jobs
    void loadSkeleton();
    QVector<QMatrix4x4> calculateSkinningMatrixPalette();

    // Allow unit tests to set the data type
#if !defined(QT_BUILD_INTERNAL)
private:
#endif
    enum SkeletonDataType {
        Unknown,
        File,
        Data
    };
#if defined(QT_BUILD_INTERNAL)
public:
    void setDataType(SkeletonDataType dataType) { m_dataType = dataType; }
#endif

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) Q_DECL_FINAL;
    void loadSkeletonFromUrl();
    void loadSkeletonFromData();
    Qt3DCore::QJoint *createFrontendJoints(const SkeletonData &skeletonData) const;
    Qt3DCore::QJoint *createFrontendJoint(const JointInfo &jointInfo) const;
    void processJointHierarchy(Qt3DCore::QNodeId jointId, int parentJointIndex, SkeletonData &skeletonData);
    void clearData();

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
    HSkeleton m_skeletonHandle; // Our own handle to set on joints

#if defined(QT_BUILD_INTERNAL)
    friend class ::tst_Skeleton;
#endif
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const Skeleton &skeleton)
{
    QDebugStateSaver saver(dbg);
    dbg << "QNodeId =" << skeleton.peerId() << endl
        << "Name =" << skeleton.name() << endl;
    return dbg;
}
#endif

} // namespace Render
} // namespace Qt3DRender


QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SKELETON_H
