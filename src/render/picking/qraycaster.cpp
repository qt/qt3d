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

#include "qraycaster.h"
#include "qraycaster_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

void QRayCasterPrivate::dispatchHits(const QRayCaster::Hits &hits)
{
    Q_Q(QRayCaster);
    m_hits = hits;
    for (int i = 0; i < m_hits.size(); i++)
        m_hits[i].setEntity(qobject_cast<Qt3DCore::QEntity *>(m_scene->lookupNode(m_hits[i].entityId())));
    bool v = q->blockNotifications(true);
    emit q->hitsChanged(m_hits);
    q->blockNotifications(v);
}

/*!
    \class Qt3DRender::QRayCaster
    \inmodule Qt3DRender

    \brief The QRayCaster...

    \sa Qt3DRender::QPickingSettings, Qt3DRender::QObjectPicker

    \since 5.11
*/

/*!
    \qmltype RayCaster
    \instantiates Qt3DRender::QRayCaster
    \inqmlmodule Qt3D.Render
    \brief The RayCaster
    \sa PickingSettings, ObjectPicker
 */


QRayCaster::QRayCaster(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QRayCasterPrivate(), parent)
{
}

QRayCaster::QRayCaster(QRayCasterPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
}

/*! \internal */
QRayCaster::~QRayCaster()
{
}

QRayCaster::RunMode QRayCaster::runMode() const
{
    Q_D(const QRayCaster);
    return d->m_runMode;
}

void QRayCaster::setRunMode(QRayCaster::RunMode runMode)
{
    Q_D(QRayCaster);
    if (d->m_runMode != runMode) {
        d->m_runMode = runMode;
        emit runModeChanged(d->m_runMode);
    }
}

QVector3D QRayCaster::origin() const
{
    Q_D(const QRayCaster);
    return d->m_origin;
}

void QRayCaster::setOrigin(const QVector3D &origin)
{
    Q_D(QRayCaster);
    if (d->m_origin != origin) {
        d->m_origin = origin;
        emit originChanged(d->m_origin);
    }
}

QVector3D QRayCaster::direction() const
{
    Q_D(const QRayCaster);
    return d->m_direction;
}

void QRayCaster::setDirection(const QVector3D &direction)
{
    Q_D(QRayCaster);
    if (d->m_direction != direction) {
        d->m_direction = direction;
        emit directionChanged(d->m_direction);
    }
}

float QRayCaster::length() const
{
    Q_D(const QRayCaster);
    return d->m_length;
}

void QRayCaster::setLength(float length)
{
    Q_D(QRayCaster);
    if (!qFuzzyCompare(d->m_length, length)) {
        d->m_length = length;
        emit lengthChanged(d->m_length);
    }
}

QRayCaster::Hits QRayCaster::hits() const
{
    Q_D(const QRayCaster);
    return d->m_hits;
}

void QRayCaster::trigger()
{
    setEnabled(true);
}

void QRayCaster::trigger(const QVector3D &origin, const QVector3D &direction, float length)
{
    setOrigin(origin);
    setDirection(direction);
    if (length >= 0.f)
        setLength(length);
    setEnabled(true);
}

/*! \internal */
void QRayCaster::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QRayCaster);
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
Qt3DCore::QNodeCreatedChangeBasePtr QRayCaster::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QRayCasterData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QRayCaster);
    data.runMode = d->m_runMode;
    data.origin = d->m_origin;
    data.direction = d->m_direction;
    data.length = d->m_length;
    return creationChange;
}

} // Qt3DRender

QT_END_NAMESPACE
