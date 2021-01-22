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

#ifndef QT3DANIMATION_ANIMATION_SKELETON_H
#define QT3DANIMATION_ANIMATION_SKELETON_H

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

#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DCore/private/sqt_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Q_AUTOTEST_EXPORT Skeleton : public BackendNode
{
public:
    Skeleton();

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    QVector<Qt3DCore::Sqt> joints() const { return  m_jointLocalPoses; }
    int jointCount() const { return m_jointLocalPoses.size(); }
    QString jointName(int jointIndex) const { return m_jointNames.at(jointIndex); }

    void setJointScale(int jointIndex, const QVector3D &scale)
    {
        m_jointLocalPoses[jointIndex].scale = scale;
    }

    QVector3D jointScale(int jointIndex) const
    {
        return m_jointLocalPoses[jointIndex].scale;
    }

    void setJointRotation(int jointIndex, const QQuaternion &rotation)
    {
        m_jointLocalPoses[jointIndex].rotation = rotation;
    }

    QQuaternion jointRotation(int jointIndex) const
    {
        return m_jointLocalPoses[jointIndex].rotation;
    }

    void setJointTranslation(int jointIndex, const QVector3D &translation)
    {
        m_jointLocalPoses[jointIndex].translation = translation;
    }

    QVector3D jointTranslation(int jointIndex) const
    {
        return m_jointLocalPoses[jointIndex].translation;
    }

#if defined(QT_BUILD_INTERNAL)
    void setJointCount(int jointCount)
    {
        m_jointNames.resize(jointCount);
        m_jointLocalPoses.resize(jointCount);
    }
    void setJointNames(const QVector<QString> &names) { m_jointNames = names; }
    QVector<QString> jointNames() const { return m_jointNames; }
    void setJointLocalPoses(const QVector<Qt3DCore::Sqt> &localPoses) { m_jointLocalPoses = localPoses; }
    QVector<Qt3DCore::Sqt> jointLocalPoses() const { return m_jointLocalPoses; }
#endif

private:
    QVector<QString> m_jointNames;
    QVector<Qt3DCore::Sqt> m_jointLocalPoses;
};

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_SKELETON_H
