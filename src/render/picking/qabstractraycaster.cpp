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

#include "qabstractraycaster.h"
#include "qabstractraycaster_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QAbstractRayCasterPrivate::QAbstractRayCasterPrivate()
    : QComponentPrivate()
{
    m_enabled = false;
}

QAbstractRayCasterPrivate *QAbstractRayCasterPrivate::get(QAbstractRayCaster *obj)
{
    return obj->d_func();
}

const QAbstractRayCasterPrivate *QAbstractRayCasterPrivate::get(const QAbstractRayCaster *obj)
{
    return obj->d_func();
}

void QAbstractRayCasterPrivate::updateHitEntites(QAbstractRayCaster::Hits &hits, Qt3DCore::QScene *scene)
{
    for (int i = 0; i < hits.size(); i++)
        hits[i].setEntity(qobject_cast<Qt3DCore::QEntity *>(scene->lookupNode(hits[i].entityId())));
}

void QAbstractRayCasterPrivate::dispatchHits(const QAbstractRayCaster::Hits &hits)
{
    Q_Q(QAbstractRayCaster);
    m_hits = hits;
    updateHitEntites(m_hits, m_scene);
    bool v = q->blockNotifications(true);
    emit q->hitsChanged(m_hits);
    q->blockNotifications(v);
}


QAbstractRayCaster::QAbstractRayCaster(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QAbstractRayCasterPrivate(), parent)
{
}

QAbstractRayCaster::QAbstractRayCaster(QAbstractRayCasterPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

/*! \internal */
QAbstractRayCaster::~QAbstractRayCaster()
{
}

QAbstractRayCaster::RunMode QAbstractRayCaster::runMode() const
{
    Q_D(const QAbstractRayCaster);
    return d->m_runMode;
}

void QAbstractRayCaster::setRunMode(QAbstractRayCaster::RunMode runMode)
{
    Q_D(QAbstractRayCaster);
    if (d->m_runMode != runMode) {
        d->m_runMode = runMode;
        emit runModeChanged(d->m_runMode);
    }
}

QAbstractRayCaster::Hits QAbstractRayCaster::hits() const
{
    Q_D(const QAbstractRayCaster);
    return d->m_hits;
}

/*! \internal */
void QAbstractRayCaster::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QAbstractRayCaster);
    Qt3DCore::QPropertyUpdatedChangePtr e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
    if (e->type() == Qt3DCore::PropertyUpdated) {
        const QByteArray propertyName = e->propertyName();
        if (propertyName == QByteArrayLiteral("hits")) {
            Hits hits = e->value().value<Hits>();
            d->dispatchHits(hits);
        }
    }

    QComponent::sceneChangeEvent(change);
}

/*! \internal */
Qt3DCore::QNodeCreatedChangeBasePtr QAbstractRayCaster::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAbstractRayCasterData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAbstractRayCaster);
    data.casterType = d->m_rayCasterType;
    data.runMode = d->m_runMode;
    data.origin = d->m_origin;
    data.direction = d->m_direction;
    data.length = d->m_length;
    data.position = d->m_position;
    return creationChange;
}

} // Qt3DRender

QT_END_NAMESPACE
