/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_SKELETONDATA_P_H
#define QT3DRENDER_RENDER_SKELETONDATA_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qstring.h>
#include <QtCore/qvector.h>
#include <Qt3DCore/private/sqt_p.h>

#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/joint_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

struct Q_AUTOTEST_EXPORT JointInfo
{
    JointInfo()
        : parentIndex(-1)
    {
    }

    explicit JointInfo(Joint *joint, int parentJointIndex)
        : inverseBindPose(joint->inverseBindMatrix())
        , parentIndex(parentJointIndex)
    {
    }

    QMatrix4x4 inverseBindPose;
    QMatrix4x4 globalPose;
    int parentIndex;
};

struct Q_AUTOTEST_EXPORT SkeletonData
{
    SkeletonData();

    void reserve(int size);

    QVector<JointInfo> joints;
    QVector<Qt3DCore::Sqt> localPoses;
    QVector<QString> jointNames;
    QHash<HJoint, int> jointIndices;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SKELETONDATA_P_H
