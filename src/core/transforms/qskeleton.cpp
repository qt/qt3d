// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qskeleton.h"
#include "qskeleton_p.h"
#include <Qt3DCore/qjoint.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QSkeletonPrivate::QSkeletonPrivate()
    : QAbstractSkeletonPrivate()
    , m_rootJoint(nullptr)
{
    m_type = Skeleton;
}

/*!
    \qmltype Skeleton
    \inqmlmodule Qt3D.Core
    \inherits AbstractSkeleton
    \nativetype Qt3DCore::QSkeleton
    \since 5.10
    \brief Holds the data for a skeleton to be used with skinned meshes.

    Use Skeleton if you wish to manually create the joints of a skeleton for
    use with skinned meshes. This is mainly of use to people creating editors,
    tooling, or dynamic skeletons. It is more common that a Qt 3D application
    would simply consume an existing skeleton and skinned mesh as created in
    a digital content creation tool such as Blender. For this use case, please
    see SkeletonLoader.
*/

/*!
    \qmlproperty Joint Skeleton::rootJoint

    Holds the root joint of the hierarchy of joints forming the skeleton.
*/

/*!
    \class Qt3DCore::QSkeleton
    \inmodule Qt3DCore
    \inherits Qt3DCore::QAbstractSkeleton
    \since 5.10
    \brief Holds the data for a skeleton to be used with skinned meshes.

    Use QSkeleton if you wish to manually create the joints of a skeleton for
    use with skinned meshes. This is mainly of use to people creating editors,
    tooling, or dynamic skeletons. It is more common that a Qt 3D application
    would simply consume an existing skeleton and skinned mesh as created in
    a digital content creation tool such as Blender. For this use case, please
    see QSkeletonLoader.
*/

/*!
    Constructs a new QSkeleton with \a parent.
*/
QSkeleton::QSkeleton(Qt3DCore::QNode *parent)
    : QAbstractSkeleton(*new QSkeletonPrivate, parent)
{
}

/*! \internal */
QSkeleton::~QSkeleton()
{
}

/*!
    \property Qt3DCore::QSkeleton::rootJoint

    Holds the root joint of the hierarchy of joints forming the skeleton.
*/
Qt3DCore::QJoint *QSkeleton::rootJoint() const
{
    Q_D(const QSkeleton);
    return d->m_rootJoint;
}

void QSkeleton::setRootJoint(Qt3DCore::QJoint *rootJoint)
{
    Q_D(QSkeleton);
    if (d->m_rootJoint != rootJoint) {
        if (d->m_rootJoint)
            d->unregisterDestructionHelper(d->m_rootJoint);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (rootJoint && !rootJoint->parent())
            rootJoint->setParent(this);
        d->m_rootJoint = rootJoint;

        // Ensures proper bookkeeping
        if (d->m_rootJoint)
            d->registerDestructionHelper(d->m_rootJoint, &QSkeleton::setRootJoint, d->m_rootJoint);

        emit rootJointChanged(rootJoint);
    }
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qskeleton.cpp"
