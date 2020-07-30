/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcuboidgeometryview.h"

#include <Qt3DExtras/qcuboidgeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype CuboidGeometryView
 * \instantiates Qt3DExtras::QCuboidGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A cuboid mesh.
 */

/*!
 * \qmlproperty real CuboidGeometryView::xExtent
 *
 * Holds the x extent of the mesh.
 */

/*!
 * \qmlproperty real CuboidGeometryView::yExtent
 *
 * Holds the y extent of the mesh.
 */

/*!
 * \qmlproperty real CuboidGeometryView::zExtent
 *
 * Holds the z extent of the mesh.
 */

/*!
 * \qmlproperty size CuboidGeometryView::yzMeshResolution
 *
 * Holds the y-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidGeometryView::xzMeshResolution
 *
 * Holds the x-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidGeometryView::xyMeshResolution
 *
 * Holds the x-y resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */

/*!
 * \class Qt3DExtras::QCuboidGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QCuboidGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DCore::QGeometryView
 *
 * \brief A cuboid mesh.
 */

/*!
 * Constructs a new QCuboidGeometryView with \a parent.
 */
QCuboidGeometryView::QCuboidGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QCuboidGeometry *geometry = new QCuboidGeometry(this);
    QObject::connect(geometry, &QCuboidGeometry::xExtentChanged, this, &QCuboidGeometryView::xExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::yExtentChanged, this, &QCuboidGeometryView::yExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::zExtentChanged, this, &QCuboidGeometryView::zExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::xyMeshResolutionChanged, this, &QCuboidGeometryView::xyMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometry::xzMeshResolutionChanged, this, &QCuboidGeometryView::xzMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometry::yzMeshResolutionChanged, this, &QCuboidGeometryView::yzMeshResolutionChanged);
    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QCuboidGeometryView::~QCuboidGeometryView()
{
}

void QCuboidGeometryView::setXExtent(float xExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setXExtent(xExtent);
}

/*!
 * \property QCuboidGeometryView::xExtent
 *
 * Holds the x extent of the mesh.
 */
float QCuboidGeometryView::xExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xExtent();
}

void QCuboidGeometryView::setYExtent(float yExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setYExtent(yExtent);
}

/*!
 * \property QCuboidGeometryView::yExtent
 *
 * Holds the y extent of the mesh.
 */
float QCuboidGeometryView::yExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->yExtent();
}

void QCuboidGeometryView::setZExtent(float zExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setZExtent(zExtent);
}

/*!
 * \property QCuboidGeometryView::zExtent
 *
 * Holds the z extent of the mesh.
 */
float QCuboidGeometryView::zExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->zExtent();
}

void QCuboidGeometryView::setYZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setYZMeshResolution(resolution);
}

/*!
 * \property QCuboidGeometryView::yzMeshResolution
 *
 * Holds the y-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */
QSize QCuboidGeometryView::yzMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->yzMeshResolution();
}

void QCuboidGeometryView::setXZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setXZMeshResolution(resolution);
}

/*!
 * \property QCuboidGeometryView::xzMeshResolution
 *
 * Holds the x-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */
QSize QCuboidGeometryView::xzMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xzMeshResolution();
}

void QCuboidGeometryView::setXYMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setXYMeshResolution(resolution);
}

/*!
 * \property QCuboidGeometryView::xyMeshResolution
 *
 * Holds the x-y resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */
QSize QCuboidGeometryView::xyMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xyMeshResolution();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE
