// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
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
