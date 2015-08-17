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

#include "rendergeometry_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qabstractattribute.h>
#include <Qt3DRenderer/qgeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderGeometry::RenderGeometry()
    : QBackendNode(ReadOnly)
    , m_verticesPerPatch(0)
    , m_geometryDirty(false)
{
}

RenderGeometry::~RenderGeometry()
{
}

void RenderGeometry::cleanup()
{
    m_verticesPerPatch = 0;
    m_attributes.clear();
    m_geometryDirty = false;
}

void RenderGeometry::updateFromPeer(QNode *peer)
{
    QGeometry *geometry = static_cast<QGeometry *>(peer);
    if (geometry != Q_NULLPTR) {
        m_attributes.reserve(geometry->attributes().size());
        Q_FOREACH (QAbstractAttribute *attribute, geometry->attributes())
            m_attributes.push_back(attribute->id());
        m_verticesPerPatch = geometry->verticesPerPatch();
        m_geometryDirty = true;
    }
}

void RenderGeometry::sceneChangeEvent(const QSceneChangePtr &e)
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
        }

    default:
        break;
    }
}

void RenderGeometry::unsetDirty()
{
    m_geometryDirty = false;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
