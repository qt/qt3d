// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/qtexture.h>

#include <Qt3DQuickRender/private/quick3dlayerfilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Quick {

Quick3DLayerFilter::Quick3DLayerFilter(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QLayer> Quick3DLayerFilter::qmlLayers()
{
    using qt_size_type = qsizetype;
    using ListContentType = QLayer;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *layer) {
        Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
        if (filter) {
            filter->parentFilter()->addLayer(layer);
        }
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
        if (filter)
            return filter->parentFilter()->layers().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
        if (filter)
            return filter->parentFilter()->layers().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
        if (filter) {
            const auto layers = filter->parentFilter()->layers();
            for (QLayer *layer : layers)
                filter->parentFilter()->removeLayer(layer);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // Quick

} // namespace Render

} // Qt3D

QT_END_NAMESPACE

#include "moc_quick3dlayerfilter_p.cpp"
