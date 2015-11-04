/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "boundingvolumedebug_p.h"
#include <Qt3DRender/qboundingvolumedebug.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

BoundingVolumeDebug::BoundingVolumeDebug()
    : QBackendNode(QBackendNode::ReadWrite)
    , m_recursive(false)
    , m_radius(0.0f)
{
}

BoundingVolumeDebug::~BoundingVolumeDebug()
{
}

void BoundingVolumeDebug::cleanup()
{
    m_recursive = false;
    m_radius = 0.0f;
    m_center = QVector3D();
}

void BoundingVolumeDebug::updateFromPeer(Qt3DCore::QNode *peer)
{
    QBoundingVolumeDebug *bvD = static_cast<QBoundingVolumeDebug *>(peer);
    if (bvD) {
        m_recursive = bvD->recursive();
    }
}

void BoundingVolumeDebug::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    const Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    const QByteArray propertyName = propertyChange->propertyName();

    if (propertyChange->type() == Qt3DCore::NodeUpdated) {
        if (propertyName == QByteArrayLiteral("recursive")) {
            m_recursive = propertyChange->value().toBool();
        }
    }
}

void BoundingVolumeDebug::setRadius(float radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
        e->setPropertyName("radius");
        e->setTargetNode(peerUuid());
        e->setValue(QVariant(radius));
        notifyObservers(e);
    }
}

void BoundingVolumeDebug::setCenter(const QVector3D &center)
{
    if (m_center != center) {
        m_center = center;
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
        e->setPropertyName("center");
        e->setTargetNode(peerUuid());
        e->setValue(QVariant::fromValue(center));
        notifyObservers(e);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
