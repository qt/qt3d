// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qskeletonloader.h"
#include "qskeletonloader_p.h"
#include <Qt3DCore/qjoint.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QSkeletonLoaderPrivate::QSkeletonLoaderPrivate()
    : QAbstractSkeletonPrivate()
    , m_source()
    , m_createJoints(false)
    , m_status(QSkeletonLoader::NotReady)
    , m_rootJoint(nullptr)
{
    m_type = SkeletonLoader;
}

void QSkeletonLoaderPrivate::setStatus(QSkeletonLoader::Status status)
{
    Q_Q(QSkeletonLoader);
    if (status != m_status) {
        m_status = status;
        const bool blocked = q->blockNotifications(true);
        emit q->statusChanged(m_status);
        q->blockNotifications(blocked);
    }
}

void QSkeletonLoaderPrivate::setRootJoint(QJoint *rootJoint)
{
    Q_Q(QSkeletonLoader);
    if (rootJoint == m_rootJoint)
        return;

    if (m_rootJoint)
        unregisterDestructionHelper(m_rootJoint);

    if (rootJoint && !rootJoint->parent())
        rootJoint->setParent(q);

    m_rootJoint = rootJoint;

    // Ensures proper bookkeeping
    if (m_rootJoint)
        registerPrivateDestructionHelper(m_rootJoint, &QSkeletonLoaderPrivate::setRootJoint);

    emit q->rootJointChanged(m_rootJoint);
}

/*!
    \qmltype SkeletonLoader
    \inqmlmodule Qt3D.Core
    \inherits AbstractSkeleton
    \instantiates Qt3DCore::QSkeletonLoader
    \since 5.10
    \brief Used to load a skeleton of joints from file.

    Use SkeletonLoader if you wish to load a whole skeleton from file rather
    than creating the joints yourself using Skeleton and Joints. Creating a
    skeleton and binding the vertices of a mesh to the skeleton is most easily
    performed in a 3D digital content creation tool such as Blender. The
    resulting skeleton and mesh can then be exported in a suitable format such
    as glTF 2 for consumption by Qt 3D.
*/

/*!
    \qmlproperty url SkeletonLoader::source

    Holds the source url from which to load the skeleton.
*/

/*!
    \qmlproperty SkeletonLoader.Status SkeletonLoader::status

    Holds the current status of skeleton loading.
*/

/*!
    \class Qt3DCore::QSkeletonLoader
    \inmodule Qt3DCore
    \inherits Qt3DCore::QAbstractSkeleton
    \since 5.10
    \brief Used to load a skeleton of joints from file.

    Use SkeletonLoader if you wish to load a whole skeleton from file rather
    than creating the joints yourself using Skeleton and Joints. Creating a
    skeleton and binding the vertices of a mesh to the skeleton is most easily
    performed in a 3D digital content creation tool such as Blender. The
    resulting skeleton and mesh can then be exported in a suitable format such
    as glTF 2 for consumption by Qt 3D.
*/

/*!
    \enum QSkeletonLoader::Status

    This enum identifies the status of skeleton.

    \value NotReady              The skeleton has not been loaded yet
    \value Ready                 The skeleton was successfully loaded
    \value Error                 An error occurred while loading the skeleton
*/
/*!
    \property Qt3DCore::QSkeletonLoader::createJointsEnabled

    \brief A boolean to indicate whether createJoints is enabled or not.
*/
/*!
    Constructs a new QSkeletonLoader with \a parent.
*/
QSkeletonLoader::QSkeletonLoader(Qt3DCore::QNode *parent)
    : QAbstractSkeleton(*new QSkeletonLoaderPrivate, parent)
{
}

/*!
    Constructs a new QSkeletonLoader with \a parent and sets the \a source.
*/
QSkeletonLoader::QSkeletonLoader(const QUrl &source, QNode *parent)
    : QAbstractSkeleton(*new QSkeletonLoaderPrivate, parent)
{
    setSource(source);
}

/*! \internal */
QSkeletonLoader::QSkeletonLoader(QSkeletonLoaderPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractSkeleton(dd, parent)
{
}

/*! \internal */
QSkeletonLoader::~QSkeletonLoader()
{
}

/*!
    \property Qt3DCore::QSkeletonLoader::source

    Holds the source url from which to load the skeleton.
*/
QUrl QSkeletonLoader::source() const
{
    Q_D(const QSkeletonLoader);
    return d->m_source;
}

/*!
    \property Qt3DCore::QSkeletonLoader::status

    Holds the current status of skeleton loading.
*/
QSkeletonLoader::Status QSkeletonLoader::status() const
{
    Q_D(const QSkeletonLoader);
    return d->m_status;
}

/*!
    Returns a boolean indicating whether CreateJoints
    is enabled or not.
    The default value is \c false.
*/
bool QSkeletonLoader::isCreateJointsEnabled() const
{
    Q_D(const QSkeletonLoader);
    return d->m_createJoints;
}
/*!
    Returns the root joint of the hierarchy of joints forming the skeleton.
*/
Qt3DCore::QJoint *QSkeletonLoader::rootJoint() const
{
    Q_D(const QSkeletonLoader);
    return d->m_rootJoint;
}

void QSkeletonLoader::setSource(const QUrl &source)
{
    Q_D(QSkeletonLoader);
    if (d->m_source == source)
        return;

    d->m_source = source;
    emit sourceChanged(source);
}

void QSkeletonLoader::setCreateJointsEnabled(bool createJoints)
{
    Q_D(QSkeletonLoader);
    if (d->m_createJoints == createJoints)
        return;

    d->m_createJoints = createJoints;
    emit createJointsEnabledChanged(createJoints);
}

void QSkeletonLoader::setRootJoint(QJoint *rootJoint)
{
    Q_D(QSkeletonLoader);
    d->setRootJoint(rootJoint);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qskeletonloader.cpp"
