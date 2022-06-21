// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qchannelmapper.h"
#include "qchannelmapper_p.h"
#include <Qt3DAnimation/qchannelmapping.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QChannelMapperPrivate::QChannelMapperPrivate()
    : Qt3DCore::QNodePrivate()
{
}

/*!
    \class Qt3DAnimation::QChannelMapper
    \inmodule Qt3DAnimation
    \brief Allows to map the channels within the clip onto properties of
    objects in the application.

*/
QChannelMapper::QChannelMapper(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QChannelMapperPrivate, parent)
{
}

QChannelMapper::QChannelMapper(QChannelMapperPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QChannelMapper::~QChannelMapper()
{
}

void QChannelMapper::addMapping(QAbstractChannelMapping *mapping)
{
    Q_ASSERT(mapping);
    Q_D(QChannelMapper);
    if (!d->m_mappings.contains(mapping)) {
        d->m_mappings.append(mapping);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(mapping, &QChannelMapper::removeMapping, d->m_mappings);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!mapping->parent())
            mapping->setParent(this);

        d->update();
    }
}

void QChannelMapper::removeMapping(QAbstractChannelMapping *mapping)
{
    Q_ASSERT(mapping);
    Q_D(QChannelMapper);
    if (!d->m_mappings.removeOne(mapping))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(mapping);
}

QList<QAbstractChannelMapping *> QChannelMapper::mappings() const
{
    Q_D(const QChannelMapper);
    return d->m_mappings;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qchannelmapper.cpp"
