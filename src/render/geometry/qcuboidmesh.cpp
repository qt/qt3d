/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcuboidmesh.h"
#include "qcuboidgeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \qmltype QCuboidMesh
 * \instantiates Qt3DRender::QCuboidMesh
 * \inqmlmodule Qt3D.Render
 * \brief A cube mesh.
 */

/*!
 * \qmlproperty float CuboidMesh::xExtent
 *
 * Holds the x extent.
 */

/*!
 * \qmlproperty float CuboidMesh::yExtent
 *
 * Holds the y extent.
 */

/*!
 * \qmlproperty float CuboidMesh::zExtent
 *
 * Holds the z extent.
 */

/*!
 * \qmlproperty size CuboidMesh::yzMeshResolution
 *
 * Holds the y-z resolution.
 */

/*!
 * \qmlproperty size CuboidMesh::xzMeshResolution
 *
 * Holds the x-z resolution.
 */

/*!
 * \qmlproperty size CuboidMesh::xyMeshResolution
 *
 * Holds the x-y resolution.
 */

/*!
 * \class Qt3DRender::QCuboidMesh
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A cube mesh.
 */

/*!
 * Constructs a new QCuboidMedh with \a parent.
 */
QCuboidMesh::QCuboidMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    QCuboidGeometry *geometry = new QCuboidGeometry(this);
    QObject::connect(geometry, &QCuboidGeometry::xExtentChanged, this, &QCuboidMesh::xExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::yExtentChanged, this, &QCuboidMesh::yExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::zExtentChanged, this, &QCuboidMesh::zExtentChanged);
    QObject::connect(geometry, &QCuboidGeometry::xyMeshResolutionChanged, this, &QCuboidMesh::xyMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometry::xzMeshResolutionChanged, this, &QCuboidMesh::xzMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometry::yzMeshResolutionChanged, this, &QCuboidMesh::yzMeshResolutionChanged);
    QGeometryRenderer::setGeometry(geometry);
}

/*!
 * Destroys this cube mesh.
 */
QCuboidMesh::~QCuboidMesh()
{
    QNode::cleanup();
}

void QCuboidMesh::setXExtent(float xExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setXExtent(xExtent);
}

/*!
 * \property QCuboidMesh::xExtent
 *
 * Holds the x extent.
 */
float QCuboidMesh::xExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xExtent();
}

void QCuboidMesh::setYExtent(float yExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setYExtent(yExtent);
}

/*!
 * \property QCuboidMesh::yExtent
 *
 * Holds the y extent.
 */
float QCuboidMesh::yExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->yExtent();
}

void QCuboidMesh::setZExtent(float zExtent)
{
    static_cast<QCuboidGeometry *>(geometry())->setZExtent(zExtent);
}

/*!
 * \property QCuboidMesh::zExtent
 *
 * Holds the z extent.
 */
float QCuboidMesh::zExtent() const
{
    return static_cast<QCuboidGeometry *>(geometry())->zExtent();
}

void QCuboidMesh::setYZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setYZMeshResolution(resolution);
}

/*!
 * \property QCuboidMesh::yzMeshResolution
 *
 * Holds the y-z resolution.
 */
QSize QCuboidMesh::yzMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->yzMeshResolution();
}

void QCuboidMesh::setXZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setXZMeshResolution(resolution);
}

/*!
 * \property QCuboidMesh::xzMeshResolution
 *
 * Holds the x-z resolution.
 */
QSize QCuboidMesh::xzMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xzMeshResolution();
}

void QCuboidMesh::setXYMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometry *>(geometry())->setXYMeshResolution(resolution);
}

/*!
 * \property QCuboidMesh::xyMeshResolution
 *
 * Holds the x-y resolution.
 */
QSize QCuboidMesh::xyMeshResolution() const
{
    return static_cast<QCuboidGeometry *>(geometry())->xyMeshResolution();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
