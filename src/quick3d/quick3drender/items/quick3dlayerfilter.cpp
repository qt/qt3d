/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

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
    return QQmlListProperty<QLayer>(this, 0,
                                    &Quick3DLayerFilter::appendLayer,
                                    &Quick3DLayerFilter::layerCount,
                                    &Quick3DLayerFilter::layerAt,
                                    &Quick3DLayerFilter::clearLayers);
}

void Quick3DLayerFilter::appendLayer(QQmlListProperty<QLayer> *list, QLayer *layer)
{
    Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
    if (filter) {
        filter->parentFilter()->addLayer(layer);
    }
}

QLayer *Quick3DLayerFilter::layerAt(QQmlListProperty<QLayer> *list, int index)
{
    Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
    if (filter) {
        return filter->parentFilter()->layers().at(index);
    }
    return 0;
}

int Quick3DLayerFilter::layerCount(QQmlListProperty<QLayer> *list)
{
    Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
    if (filter) {
        return filter->parentFilter()->layers().count();
    }
    return 0;
}

void Quick3DLayerFilter::clearLayers(QQmlListProperty<QLayer> *list)
{
    Quick3DLayerFilter *filter = qobject_cast<Quick3DLayerFilter *>(list->object);
    if (filter) {
        const auto layers = filter->parentFilter()->layers();
        for (QLayer *layer : layers)
            filter->parentFilter()->removeLayer(layer);
    }
}

} // Quick

} // namespace Render

} // Qt3D

QT_END_NAMESPACE

#include "moc_quick3dlayerfilter_p.cpp"
