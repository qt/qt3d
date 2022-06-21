/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/

#include "qabstractskeleton.h"
#include "qabstractskeleton_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAbstractSkeletonPrivate::QAbstractSkeletonPrivate()
    : Qt3DCore::QNodePrivate()
    , m_type(QSkeletonCreatedChangeBase::Skeleton)
    , m_jointCount(0)
{
}

/*!
    \internal
 */
const QAbstractSkeletonPrivate *QAbstractSkeletonPrivate::get(const QAbstractSkeleton *q)
{
    return q->d_func();
}

/*!
    \internal
 */
QAbstractSkeletonPrivate *QAbstractSkeletonPrivate::get(QAbstractSkeleton *q)
{
    return q->d_func();
}

/*!
    \qmltype AbstractSkeleton
    \inqmlmodule Qt3D.Core
    \inherits Node
    \instantiates Qt3DCore::QAbstractSkeleton
    \since 5.10
    \brief A skeleton contains the joints for a skinned mesh.

    Do not use this class directly. You should use SkeletonLoader if loading
    skeleton data from a file (most likely) or Skeleton if creating the
    skeleton and skinned mesh data yourself (mainly for people creating
    editors or tooling).
*/

/*!
    \class Qt3DCore::QAbstractSkeleton
    \inmodule Qt3DCore
    \inherits Qt3DCore::QNode
    \since 5.10
    \brief A skeleton contains the joints for a skinned mesh.

    Do not use this class directly. You should use QSkeletonLoader if loading
    skeleton data from a file (most likely) or QSkeleton if creating the
    skeleton and skinned mesh data yourself (mainly for people creating
    editors or tooling).
*/

/*! \internal */
QAbstractSkeleton::QAbstractSkeleton(QAbstractSkeletonPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

/*! \internal */
QAbstractSkeleton::~QAbstractSkeleton()
{
}

/*! \internal */
void QAbstractSkeleton::sceneChangeEvent(const QSceneChangePtr &change)
{
    // TODO Unused remove in Qt6
    Q_UNUSED(change)
}

/*!
    \property Qt3DCore::QAbstractSkeleton::jointCount

    Holds the number of joints contained in the skeleton
*/
int QAbstractSkeleton::jointCount() const
{
    Q_D(const QAbstractSkeleton);
    return d->m_jointCount;
}

void QAbstractSkeletonPrivate::setJointCount(int jointCount)
{
    Q_Q(QAbstractSkeleton);
    if (m_jointCount == jointCount)
        return;
    m_jointCount = jointCount;
    const bool block = q->blockNotifications(true);
    emit q->jointCountChanged(jointCount);
    q->blockNotifications(block);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qabstractskeleton.cpp"
