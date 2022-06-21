/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "quick3dchannelmapper_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {
namespace Quick {

Quick3DChannelMapper::Quick3DChannelMapper(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractChannelMapping> Quick3DChannelMapper::qmlMappings()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QAbstractChannelMapping;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *mapping) {
        Quick3DChannelMapper *extension = qobject_cast<Quick3DChannelMapper *>(list->object);
        extension->parentMapper()->addMapping(mapping);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DChannelMapper *extension = qobject_cast<Quick3DChannelMapper *>(list->object);
        return extension->parentMapper()->mappings().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DChannelMapper *extension = qobject_cast<Quick3DChannelMapper *>(list->object);
        return extension->parentMapper()->mappings().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DChannelMapper *extension = qobject_cast<Quick3DChannelMapper *>(list->object);
        const auto mappings = extension->parentMapper()->mappings();
        for (QAbstractChannelMapping *mapping : mappings)
            extension->parentMapper()->removeMapping(mapping);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3dchannelmapper_p.cpp"
