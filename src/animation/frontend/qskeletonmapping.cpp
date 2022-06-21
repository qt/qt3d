// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include "moc_qskeletonmapping.cpp"
