/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DQuickRender/private/quick3dscreenraycaster_p.h>
#include <Qt3DQuickRender/private/quick3dscreenraycaster_p_p.h>
#include <Qt3DQuickRender/private/quick3draycaster_p_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/QEntity>

#include <QQmlEngine>
#include <QJSValue>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DScreenRayCaster::Quick3DScreenRayCaster(QObject *parent)
    : QScreenRayCaster(*new Quick3DScreenRayCasterPrivate(), qobject_cast<Qt3DCore::QNode *>(parent))
{
}

QQmlListProperty<Qt3DRender::QLayer> Qt3DRender::Render::Quick::Quick3DScreenRayCaster::qmlLayers()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

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

#include "moc_quick3dscreenraycaster_p.cpp"
