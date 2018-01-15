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

#include "raycaster_p.h"
#include "qpickevent.h"
#include "renderer_p.h"
#include <Qt3DRender/qraycaster.h>
#include <Qt3DRender/private/qraycaster_p.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

RayCaster::RayCaster()
    : BackendNode(QBackendNode::ReadWrite)
{
}

RayCaster::~RayCaster()
{
}

QRayCaster::RunMode RayCaster::runMode() const
{
    return m_runMode;
}

QVector3D RayCaster::origin() const
{
    return m_origin;
}

QVector3D RayCaster::direction() const
{
    return m_direction;
}

float RayCaster::length() const
{
    return m_length;
}

void RayCaster::cleanup()
{
    BackendNode::setEnabled(false);
    m_runMode = QRayCaster::SingleShot;
    m_direction = QVector3D(0.f, 0.f, 1.f);
    m_origin = {};
    m_length = 0.f;
}

void RayCaster::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QRayCasterData>>(change);
    const auto &data = typedChange->data;
    m_runMode = data.runMode;
    m_origin = data.origin;
    m_direction = data.direction;
    m_length = data.length;
}

void RayCaster::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::PropertyUpdated) {
        const Qt3DCore::QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);

        if (propertyChange->propertyName() == QByteArrayLiteral("origin")) {
            m_origin = propertyChange->value().value<QVector3D>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("direction")) {
            m_direction = propertyChange->value().value<QVector3D>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("length")) {
            m_length = propertyChange->value().toFloat();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("runMode")) {
            m_runMode = propertyChange->value().value<QRayCaster::RunMode>();
        }

        markDirty(AbstractRenderer::AllDirty);
    }

    BackendNode::sceneChangeEvent(e);
}

void RayCaster::dispatchHits(const QRayCaster::Hits &hits)
{
    auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
    e->setPropertyName("hits");
    e->setValue(QVariant::fromValue(hits));
    notifyObservers(e);

    if (m_runMode == QRayCaster::SingleShot) {
        setEnabled(false);
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("enabled");
        e->setValue(false);
        notifyObservers(e);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
