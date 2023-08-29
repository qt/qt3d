// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qplanemesh.h"

#include <Qt3DExtras/qplanegeometryview.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype PlaneMesh
 * \instantiates Qt3DExtras::QPlaneMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A square planar mesh.
 */

/*!
 * \qmlproperty real PlaneMesh::width
 *
 * Holds the plane width.
 */

/*!
 * \qmlproperty real PlaneMesh::height
 *
 * Holds the plane height.
 */

/*!
 * \qmlproperty size PlaneMesh::meshResolution
 *
 * Holds the plane resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the mesh in the respective dimensions.
 */

/*!
 * \qmlproperty bool PlaneMesh::mirrored
 * \since 5.9
 *
 * Controls if the UV coordinates of the plane should be flipped vertically.
 */

/*!
 * \class Qt3DExtras::QPlaneMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QPlaneMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A square planar mesh.
 */

/*!
 * Constructs a new QPlaneMesh with \a parent.
 */
QPlaneMesh::QPlaneMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QPlaneGeometryView *geometry = new QPlaneGeometryView(this);
    QObject::connect(geometry, &QPlaneGeometryView::widthChanged, this, &QPlaneMesh::widthChanged);
    QObject::connect(geometry, &QPlaneGeometryView::heightChanged, this, &QPlaneMesh::heightChanged);
    QObject::connect(geometry, &QPlaneGeometryView::meshResolutionChanged, this, &QPlaneMesh::meshResolutionChanged);
    QObject::connect(geometry, &QPlaneGeometryView::mirroredChanged, this, &QPlaneMesh::mirroredChanged);
    setView(geometry);
}

/*! \internal */
QPlaneMesh::~QPlaneMesh()
{
}

void QPlaneMesh::setWidth(float width)
{
    static_cast<QPlaneGeometryView *>(view())->setWidth(width);
}

/*!
 * \property QPlaneMesh::width
 *
 * Holds the plane width.
 */
float QPlaneMesh::width() const
{
    return static_cast<QPlaneGeometryView *>(view())->width();
}

void QPlaneMesh::setHeight(float height)
{
    static_cast<QPlaneGeometryView *>(view())->setHeight(height);
}

/*!
 * \property QPlaneMesh::height
 *
 * Holds the plane height.
 */
float QPlaneMesh::height() const
{
    return static_cast<QPlaneGeometryView *>(view())->height();
}

void QPlaneMesh::setMeshResolution(const QSize &resolution)
{
    static_cast<QPlaneGeometryView *>(view())->setMeshResolution(resolution);
}

/*!
 * \property QPlaneMesh::meshResolution
 *
 * Holds the plane resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the mesh in the respective dimensions.
 */
QSize QPlaneMesh::meshResolution() const
{
    return static_cast<QPlaneGeometryView *>(view())->meshResolution();
}

void QPlaneMesh::setMirrored(bool mirrored)
{
    static_cast<QPlaneGeometryView *>(view())->setMirrored(mirrored);
}

/*!
 * \property QPlaneMesh::mirrored
 * \since 5.9
 *
 * Controls if the UV coordinates of the plane should be flipped vertically.
 */
bool QPlaneMesh::mirrored() const
{
    return static_cast<QPlaneGeometryView *>(view())->mirrored();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qplanemesh.cpp"
