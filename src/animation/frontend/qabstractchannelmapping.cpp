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
