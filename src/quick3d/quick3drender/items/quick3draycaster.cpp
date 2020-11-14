/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <Qt3DQuickRender/private/quick3draycaster_p.h>
#include <Qt3DQuickRender/private/quick3draycaster_p_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

void Quick3DRayCasterPrivate::appendLayer(QQmlListProperty<QLayer> *list, QLayer *layer)
{
    QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
    if (filter)
        filter->addLayer(layer);
}

QLayer *Quick3DRayCasterPrivate::layerAt(QQmlListProperty<QLayer> *list, qsizetype index)
{
    QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
    if (filter)
        return filter->layers().at(index);
    return nullptr;
}

qsizetype Quick3DRayCasterPrivate::layerCount(QQmlListProperty<QLayer> *list)
{
    QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
    if (filter)
        return int(filter->layers().size());
    return 0;
}

void Quick3DRayCasterPrivate::clearLayers(QQmlListProperty<QLayer> *list)
{
    QAbstractRayCaster *filter = qobject_cast<QAbstractRayCaster *>(list->object);
    if (filter) {
        const auto layers = filter->layers();
        for (QLayer *layer : layers)
            filter->removeLayer(layer);
    }
}

Quick3DRayCaster::Quick3DRayCaster(QObject *parent)
    : QRayCaster(*new Quick3DRayCasterPrivate(), qobject_cast<Qt3DCore::QNode *>(parent))
{
}

QQmlListProperty<Qt3DRender::QLayer> Qt3DRender::Render::Quick::Quick3DRayCaster::qmlLayers()
{
    return QQmlListProperty<QLayer>(this, nullptr,
                                    &Quick3DRayCasterPrivate::appendLayer,
                                    &Quick3DRayCasterPrivate::layerCount,
                                    &Quick3DRayCasterPrivate::layerAt,
                                    &Quick3DRayCasterPrivate::clearLayers);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
