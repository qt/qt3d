// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QJOINT_P_H
#define QT3DCORE_QJOINT_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qjoint.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QJoint;

class Q_3DCORE_PRIVATE_EXPORT QJointPrivate : public QNodePrivate
{
public:
    QJointPrivate();

    Q_DECLARE_PUBLIC(QJoint)

    QMatrix4x4 m_inverseBindMatrix;
    QList<QJoint *> m_childJoints;
    QQuaternion m_rotation;
    QVector3D m_translation;
    QVector3D m_scale;
    QString m_name;

    // Not sent to backend. Purely internal convenience
    QVector3D m_eulerRotationAngles;
};

struct QJointData
{
    QMatrix4x4 inverseBindMatrix;
    QNodeIdVector childJointIds;
    QQuaternion rotation;
    QVector3D translation;
    QVector3D scale;
    QString name;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QJOINT_P_H
