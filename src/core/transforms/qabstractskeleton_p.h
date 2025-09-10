// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTSKELETON_P_H
#define QT3DCORE_QABSTRACTSKELETON_P_H

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

#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/sqt_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORE_PRIVATE_EXPORT QAbstractSkeletonPrivate : public Qt3DCore::QNodePrivate
{
public:
    enum SkeletonType {
        Skeleton = 0,
        SkeletonLoader
    };

    QAbstractSkeletonPrivate();

    void setJointCount(int jointCount);

    Q_DECLARE_PUBLIC(QAbstractSkeleton)
    static const QAbstractSkeletonPrivate *get(const QAbstractSkeleton *q);
    static QAbstractSkeletonPrivate *get(QAbstractSkeleton *q);

    SkeletonType m_type;

    int m_jointCount;
    QVector<Sqt> m_localPoses;
    QVector<QString> m_jointNames;
};

} // namespace Qt3DCore


QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTSKELETON_P_H
