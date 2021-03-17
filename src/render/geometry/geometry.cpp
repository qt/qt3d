/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "geometry_p.h"
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/private/qgeometry_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Geometry::Geometry()
    : BackendNode(ReadWrite)
    , m_geometryDirty(false)
    , m_shouldNotifyMinExtentChanged(false)
    , m_shouldNotifyMaxExtentChanged(false)
{
}

Geometry::~Geometry()
{
}

void Geometry::cleanup()
{
    QBackendNode::setEnabled(false);
    m_attributes.clear();
    m_geometryDirty = false;
    m_boundingPositionAttribute = Qt3DCore::QNodeId();
    m_min = QVector3D();
    m_max = QVector3D();
    m_shouldNotifyMinExtentChanged = false;
    m_shouldNotifyMaxExtentChanged = false;
}

void Geometry::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QGeometry *node = qobject_cast<const QGeometry *>(frontEnd);
    if (!node)
        return;

    m_geometryDirty |= firstTime;

    QNodeIdVector attribs = qIdsForNodes(node->attributes());
    std::sort(std::begin(attribs), std::end(attribs));
    if (m_attributes != attribs) {
        m_attributes = attribs;
        m_geometryDirty = true;
    }

    if ((node->boundingVolumePositionAttribute() && node->boundingVolumePositionAttribute()->id() != m_boundingPositionAttribute) ||
        // Note: doesn't set dirtyness as this parameter changing doesn't need a new VAO update.
        (!node->boundingVolumePositionAttribute() && !m_boundingPositionAttribute.isNull())) {
        m_boundingPositionAttribute = node->boundingVolumePositionAttribute() ? node->boundingVolumePositionAttribute()->id() : QNodeId{};
    }

    markDirty(AbstractRenderer::GeometryDirty);
}

void Geometry::unsetDirty()
{
    m_geometryDirty = false;
}

// Called from calcboundingvolumejob (in a QtConcurrent thead (can't send
// update changes from such a thread))
void Geometry::updateExtent(const QVector3D &min, const QVector3D &max)
{
    // Send notification to frontend
    if (m_min != min) {
        m_min = min;
        m_shouldNotifyMinExtentChanged = true;
    }

    if (m_max != max) {
        m_max = max;
        m_shouldNotifyMaxExtentChanged = true;
    }
}

// Called from calcboundingvolumejob after all bounding volumes have been
// updated (in an aspect thread)
void Geometry::notifyExtentChanged()
{
    if (m_shouldNotifyMinExtentChanged || m_shouldNotifyMaxExtentChanged) {
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
        change->setPropertyName("extent");
        change->setValue(QVariant::fromValue(QPair<QVector3D, QVector3D>(m_min, m_max)));
        notifyObservers(change);
        m_shouldNotifyMinExtentChanged = false;
        m_shouldNotifyMaxExtentChanged = false;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
