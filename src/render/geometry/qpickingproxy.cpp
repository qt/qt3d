/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qpickingproxy.h"
#include "qpickingproxy_p.h"

#include <private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QPickingProxyPrivate::QPickingProxyPrivate()
    : Qt3DCore::QBoundingVolumePrivate()
{
    m_primaryProvider = false;
}

QPickingProxyPrivate::~QPickingProxyPrivate()
{
}

void QPickingProxyPrivate::setView(QGeometryView *view)
{
    Q_Q(QPickingProxy);
    if (m_view == view)
        return;

    if (m_view)
        m_view->disconnect(q);

    QBoundingVolumePrivate::setView(view);

    // Ensures proper bookkeeping
    if (m_view) {
        QObject::connect(view, &QGeometryView::instanceCountChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::vertexCountChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::indexOffsetChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::firstInstanceChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::firstVertexChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::indexBufferByteOffsetChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::restartIndexValueChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::verticesPerPatchChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::primitiveRestartEnabledChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::geometryChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::primitiveTypeChanged, q, [this]() { update(); });
    }
}

/*!
    \qmltype PickingProxy
    \instantiates Qt3DRender::QPickingProxy
    \inqmlmodule Qt3D.Render
    \inherits Component3D
    \since 2.16
    \brief Can be used to provide an alternate GeometryView used only for picking.

    Picking can be an expensive operation, especially if the mesh has a lot of vertices.
    PickProxy can be used to provide an alternative geometry, usually with fewer primitives,
    which will be used for picking, while the GeometryRenderer instance will be used
    for rendering.

    \note Do not use a PickingProxy if the application requires picking coordinates
    to match the rendered mesh.

    \note The picking algorithm uses a bounding volume hierarchy to optimize out
    entities who's bounding volume does not intersect the picking ray. For that hierarchy,
    the bounding volume of the renderered entity is used (or one explicitly set using a
    BoundingVolume component) will be used rather than the one of the proxy.
 */

/*!
    \class Qt3DRender::QPickingProxy
    \inmodule Qt3DRender
    \since 6.0
    \brief Can be used to provide an alternate QGeometryView used only for picking.

    Picking can be an expensive operation, especially if the mesh has a lot of vertices.
    QPickProxy can be used to provide an alternative geometry, usually with fewer primitives,
    which will be used for picking, while the GeometryRenderer instance will be used
    for rendering.

    \note Do not use a QPickingProxy if the application requires picking coordinates
    to match the rendered mesh.

    \note The picking algorithm uses a bounding volume hierarchy to optimize out
    entities who's bounding volume does not intersect the picking ray. For that hierarchy,
    the bounding volume of the renderered entity is used (or one explicitly set using a
    QBoundingVolume component) will be used rather than the one of the proxy.
 */


/*!
    Constructs a new QPickingProxy with \a parent.
 */
QPickingProxy::QPickingProxy(QNode *parent)
    : Qt3DCore::QBoundingVolume(*new QPickingProxyPrivate(), parent)
{
}

/*!
    \internal
 */
QPickingProxy::~QPickingProxy()
{
}

/*!
    \internal
 */
QPickingProxy::QPickingProxy(QPickingProxyPrivate &dd, QNode *parent)
    : Qt3DCore::QBoundingVolume(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE
