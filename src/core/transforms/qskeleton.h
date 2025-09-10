// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QSKELETON_H
#define QT3DCORE_QSKELETON_H

#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSkeletonPrivate;

class Q_3DCORESHARED_EXPORT QSkeleton : public QAbstractSkeleton
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QJoint* rootJoint READ rootJoint WRITE setRootJoint NOTIFY rootJointChanged)

public:
    QSkeleton(Qt3DCore::QNode *parent = nullptr);
    ~QSkeleton();

    Qt3DCore::QJoint *rootJoint() const;

public Q_SLOTS:
    void setRootJoint(Qt3DCore::QJoint *rootJoint);

Q_SIGNALS:
    void rootJointChanged(Qt3DCore::QJoint *rootJoint);

private:
    Q_DECLARE_PRIVATE(QSkeleton)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSKELETON_H
