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

#include "geometry_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qabstractattribute.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qboundingvolumespecifier.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Geometry::Geometry()
    : QBackendNode(ReadOnly)
    , m_verticesPerPatch(0)
    , m_geometryDirty(false)
{
}

Geometry::~Geometry()
{
}

void Geometry::cleanup()
{
    m_verticesPerPatch = 0;
    m_attributes.clear();
    m_geometryDirty = false;
    m_boundingPositionAttribute = Qt3DCore::QNodeId();
}

void Geometry::updateFromPeer(Qt3DCore::QNode *peer)
{
    QGeometry *geometry = static_cast<QGeometry *>(peer);
    if (geometry != Q_NULLPTR) {
        m_attributes.reserve(geometry->attributes().size());
        Q_FOREACH (QAbstractAttribute *attribute, geometry->attributes())
            m_attributes.push_back(attribute->id());
        m_verticesPerPatch = geometry->verticesPerPatch();
        m_geometryDirty = true;
        if (geometry->boundingVolumeSpecifier()->positionAttribute() != Q_NULLPTR)
            m_boundingPositionAttribute = geometry->boundingVolumeSpecifier()->positionAttribute()->id();
    }
}

void Geometry::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    QByteArray propertyName = propertyChange->propertyName();

    switch (e->type()) {
    case NodeAdded: {
        if (propertyName == QByteArrayLiteral("attribute")) {
            m_attributes.push_back(propertyChange->value().value<QNodeId>());
            m_geometryDirty = true;
        }
        break;
    }

    case NodeRemoved: {
        if (propertyName == QByteArrayLiteral("attribute")) {
            m_attributes.removeOne(propertyChange->value().value<QNodeId>());
            m_geometryDirty = true;
        }
        break;
    }

    case NodeUpdated:
        if (propertyName == QByteArrayLiteral("verticesPerPatch")) {
            m_verticesPerPatch = propertyChange->value().value<int>();
            break;

            // Note: doesn't set dirtyness as this parameter changing doesn't need
            // a new VAO update.
        } else if (propertyName == QByteArrayLiteral("boundingVolumeSpecifierPositionAttribute")) {
            m_boundingPositionAttribute = propertyChange->value().value<Qt3DCore::QNodeId>();
            break;
        }

    default:
        break;
    }
}

void Geometry::unsetDirty()
{
    m_geometryDirty = false;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
