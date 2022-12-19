// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/quick3draycaster_p.h>
#include <Qt3DQuickRender/private/quick3draycaster_p_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DRayCaster::Quick3DRayCaster(QObject *parent)
    : QRayCaster(*new Quick3DRayCasterPrivate(), qobject_cast<Qt3DCore::QNode *>(parent))
{
}

QQmlListProperty<Qt3DRender::QLayer> Qt3DRender::Render::Quick::Quick3DRayCaster::qmlLayers()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DRender::QLayer;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *layer) {
        QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
        if (filter)
            filter->addLayer(layer);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
        if (filter)
            return int(filter->layers().size());
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
        if (filter)
            return filter->layers().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
        if (filter) {
            const auto layers = filter->layers();
            for (QLayer *layer : layers)
                filter->removeLayer(layer);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3draycaster_p.cpp"
