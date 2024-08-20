// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcuboidmesh.h"

#include <Qt3DExtras/qcuboidgeometryview.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype CuboidMesh
 * \nativetype Qt3DExtras::QCuboidMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A cuboid mesh.
 */

/*!
 * \qmlproperty real CuboidMesh::xExtent
 *
 * Holds the x extent of the mesh.
 */

/*!
 * \qmlproperty real CuboidMesh::yExtent
 *
 * Holds the y extent of the mesh.
 */

/*!
 * \qmlproperty real CuboidMesh::zExtent
 *
 * Holds the z extent of the mesh.
 */

/*!
 * \qmlproperty size CuboidMesh::yzMeshResolution
 *
 * Holds the y-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidMesh::xzMeshResolution
 *
 * Holds the x-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidMesh::xyMeshResolution
 *
 * Holds the x-y resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */

/*!
 * \class Qt3DExtras::QCuboidMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QCuboidMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A cuboid mesh.
 */

/*!
 * Constructs a new QCuboidMesh with \a parent.
 */
QCuboidMesh::QCuboidMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QCuboidGeometryView *geometry = new QCuboidGeometryView(this);
    QObject::connect(geometry, &QCuboidGeometryView::xExtentChanged, this, &QCuboidMesh::xExtentChanged);
    QObject::connect(geometry, &QCuboidGeometryView::yExtentChanged, this, &QCuboidMesh::yExtentChanged);
    QObject::connect(geometry, &QCuboidGeometryView::zExtentChanged, this, &QCuboidMesh::zExtentChanged);
    QObject::connect(geometry, &QCuboidGeometryView::xyMeshResolutionChanged, this, &QCuboidMesh::xyMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometryView::xzMeshResolutionChanged, this, &QCuboidMesh::xzMeshResolutionChanged);
    QObject::connect(geometry, &QCuboidGeometryView::yzMeshResolutionChanged, this, &QCuboidMesh::yzMeshResolutionChanged);
    setView(geometry);
}

/*! \internal */
QCuboidMesh::~QCuboidMesh()
{
}

void QCuboidMesh::setXExtent(float xExtent)
{
    static_cast<QCuboidGeometryView *>(view())->setXExtent(xExtent);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::xExtent
 *
 * Holds the x extent of the mesh.
 */
float QCuboidMesh::xExtent() const
{
    return static_cast<QCuboidGeometryView *>(view())->xExtent();
}

void QCuboidMesh::setYExtent(float yExtent)
{
    static_cast<QCuboidGeometryView *>(view())->setYExtent(yExtent);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::yExtent
 *
 * Holds the y extent of the mesh.
 */
float QCuboidMesh::yExtent() const
{
    return static_cast<QCuboidGeometryView *>(view())->yExtent();
}

void QCuboidMesh::setZExtent(float zExtent)
{
    static_cast<QCuboidGeometryView *>(view())->setZExtent(zExtent);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::zExtent
 *
 * Holds the z extent of the mesh.
 */
float QCuboidMesh::zExtent() const
{
    return static_cast<QCuboidGeometryView *>(view())->zExtent();
}

void QCuboidMesh::setYZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometryView *>(view())->setYZMeshResolution(resolution);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::yzMeshResolution
 *
 * Holds the y-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */
QSize QCuboidMesh::yzMeshResolution() const
{
    return static_cast<QCuboidGeometryView *>(view())->yzMeshResolution();
}

void QCuboidMesh::setXZMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometryView *>(view())->setXZMeshResolution(resolution);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::xzMeshResolution
 *
 * Holds the x-z resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */
QSize QCuboidMesh::xzMeshResolution() const
{
    return static_cast<QCuboidGeometryView *>(view())->xzMeshResolution();
}

void QCuboidMesh::setXYMeshResolution(const QSize &resolution)
{
    static_cast<QCuboidGeometryView *>(view())->setXYMeshResolution(resolution);
}

/*!
 * \property Qt3DExtras::QCuboidMesh::xyMeshResolution
 *
 * Holds the x-y resolution of the mesh.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */
QSize QCuboidMesh::xyMeshResolution() const
{
    return static_cast<QCuboidGeometryView *>(view())->xyMeshResolution();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qcuboidmesh.cpp"
