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
******************************************************************************/

#include "qskeletonmapping.h"
#include "qskeletonmapping_p.h"
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QSkeletonMappingPrivate::QSkeletonMappingPrivate()
    : QAbstractChannelMappingPrivate()
    , m_skeleton(nullptr)
{
    m_mappingType = QAbstractChannelMappingPrivate::SkeletonMapping;
}

QSkeletonMapping::QSkeletonMapping(Qt3DCore::QNode *parent)
    : QAbstractChannelMapping(*new QSkeletonMappingPrivate, parent)
{
}

QSkeletonMapping::QSkeletonMapping(QSkeletonMappingPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractChannelMapping(dd, parent)
{
}

QSkeletonMapping::~QSkeletonMapping()
{
}

Qt3DCore::QAbstractSkeleton *QSkeletonMapping::skeleton() const
{
    Q_D(const QSkeletonMapping);
    return d->m_skeleton;
}

void QSkeletonMapping::setSkeleton(Qt3DCore::QAbstractSkeleton *skeleton)
{
    Q_D(QSkeletonMapping);
    if (d->m_skeleton == skeleton)
        return;

    if (d->m_skeleton)
        d->unregisterDestructionHelper(d->m_skeleton);

    if (skeleton && !skeleton->parent())
        skeleton->setParent(this);
    d->m_skeleton = skeleton;

    // Ensures proper bookkeeping
    if (d->m_skeleton)
        d->registerDestructionHelper(d->m_skeleton, &QSkeletonMapping::setSkeleton, d->m_skeleton);

    emit skeletonChanged(skeleton);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
