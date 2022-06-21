// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractchannelmapping.h"
#include "qabstractchannelmapping_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAbstractChannelMappingPrivate::QAbstractChannelMappingPrivate()
    : Qt3DCore::QNodePrivate()
    , m_mappingType(ChannelMapping)
{
}

/*! \internal */
const QAbstractChannelMappingPrivate *QAbstractChannelMappingPrivate::get(const QAbstractChannelMapping *q)
{
    return q->d_func();
}

QAbstractChannelMapping::QAbstractChannelMapping(QAbstractChannelMappingPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QAbstractChannelMapping::~QAbstractChannelMapping()
{
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qabstractchannelmapping.cpp"
