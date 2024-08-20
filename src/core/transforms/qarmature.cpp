// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qarmature.h"
#include "qarmature_p.h"
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QArmaturePrivate::QArmaturePrivate()
    : Qt3DCore::QComponentPrivate()
    , m_skeleton(nullptr)
{
}

/*!
    \qmltype Armature
    \inqmlmodule Qt3D.Core
    \inherits Component3D
    \nativetype Qt3DCore::QArmature
    \since 5.10
    \brief Used to calculate skinning transform matrices and set them on shaders.

    The Armature component is aggregated by entities to give them the ability to
    calculate the palette of skinning transform matrices needed to properly
    render skinned meshes.

    Each vertex in a skinned mesh is associated (bound) to up to 4 joints in a
    skeleton. For each joint affecting a vertex the mesh also provides a weight
    which determines the level of influence of the corresponding joint. The
    skinning palette used for performing the transformation of skinned vertices
    is provided by the Armature and is calculated from the joints contained in
    the referenced skeleton.

    Updating the local transform of a joint results in the skinning matrices
    being recalculated and the skinned mesh vertices bound to that joint moving
    accordingly.
*/

/*!
    \qmlproperty AbstractSkeleton Armature::skeleton

    Holds the skeleton used to calculate the skinning transform matrix palette.
*/

/*!
    \class Qt3DCore::QArmature
    \inmodule Qt3DCore
    \inherits Qt3DCore::QComponent
    \since 5.10
    \brief Used to calculate skinning transform matrices and set them on shaders.

    The Armature component is aggregated by entities to give them the ability to
    calculate the palette of skinning transform matrices needed to properly
    render skinned meshes.

    Each vertex in a skinned mesh is associated (bound) to up to 4 joints in a
    skeleton. For each joint affecting a vertex the mesh also provides a weight
    which determines the level of influence of the corresponding joint. The
    skinning palette used for performing the transformation of skinned vertices
    is provided by the Armature and is calculated from the joints contained in
    the referenced skeleton.

    Updating the local transform of a joint results in the skinning matrices
    being recalculated and the skinned mesh vertices bound to that joint moving
    accordingly.
*/

/*!
    Constructs a new QArmature with \a parent.
*/
QArmature::QArmature(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QArmaturePrivate, parent)
{
}

/*! \internal */
QArmature::QArmature(QArmaturePrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

/*! \internal */
QArmature::~QArmature()
{
}

/*!
    \property Qt3DCore::QArmature::skeleton

    Holds the skeleton used to calculate the skinning transform matrix palette.
*/
Qt3DCore::QAbstractSkeleton *QArmature::skeleton() const
{
    Q_D(const QArmature);
    return d->m_skeleton;
}

void QArmature::setSkeleton(Qt3DCore::QAbstractSkeleton *skeleton)
{
    Q_D(QArmature);
    if (d->m_skeleton != skeleton) {
        if (d->m_skeleton)
            d->unregisterDestructionHelper(d->m_skeleton);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (skeleton && !skeleton->parent())
            skeleton->setParent(this);
        d->m_skeleton = skeleton;

        // Ensures proper bookkeeping
        if (d->m_skeleton)
            d->registerDestructionHelper(d->m_skeleton, &QArmature::setSkeleton, d->m_skeleton);

        emit skeletonChanged(skeleton);
    }
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qarmature.cpp"
