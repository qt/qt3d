/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#include "qchannelmapper.h"
#include "qchannelmapper_p.h"
#include <Qt3DCore/qscenechange.h>
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

        d->updateNode(mapping, "mappings", Qt3DCore::PropertyValueAdded);
    }
}

void QChannelMapper::removeMapping(QAbstractChannelMapping *mapping)
{
    Q_ASSERT(mapping);
    Q_D(QChannelMapper);
    if (!d->m_mappings.removeOne(mapping))
        return;
    d->updateNode(mapping, "mappings", Qt3DCore::PropertyValueRemoved);
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(mapping);
}

QVector<QAbstractChannelMapping *> QChannelMapper::mappings() const
{
    Q_D(const QChannelMapper);
    return d->m_mappings;
}

Qt3DCore::QNodeCreatedChangeBasePtr QChannelMapper::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QChannelMapperData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QChannelMapper);
    data.mappingIds = Qt3DCore::qIdsForNodes(d->m_mappings);
    return creationChange;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
