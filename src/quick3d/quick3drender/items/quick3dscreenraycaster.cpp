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
****************************************************************************/

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

void Quick3DScreenRayCasterPrivate::dispatchHits(const QAbstractRayCaster::Hits &hits)
{
    m_hits = hits;
    updateHitEntites(m_hits, m_scene);

    Q_Q(Quick3DScreenRayCaster);
    if (!m_engine)
        m_engine = qmlEngine(q->parent());

    m_jsHits = Quick3DRayCasterPrivate::convertHits(m_hits, m_engine);
    emit q->hitsChanged(m_jsHits);
}

Quick3DScreenRayCaster::Quick3DScreenRayCaster(QObject *parent)
    : QScreenRayCaster(*new Quick3DScreenRayCasterPrivate(), qobject_cast<Qt3DCore::QNode *>(parent))
{
}

QJSValue Quick3DScreenRayCaster::hits() const
{
    Q_D(const Quick3DScreenRayCaster);
    return d->m_jsHits;
}

QQmlListProperty<Qt3DRender::QLayer> Qt3DRender::Render::Quick::Quick3DScreenRayCaster::qmlLayers()
{
    return QQmlListProperty<QLayer>(this, 0,
                                    &Quick3DRayCasterPrivate::appendLayer,
                                    &Quick3DRayCasterPrivate::layerCount,
                                    &Quick3DRayCasterPrivate::layerAt,
                                    &Quick3DRayCasterPrivate::clearLayers);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
