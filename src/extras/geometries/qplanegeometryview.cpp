// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qplanegeometryview.h"

#include <Qt3DExtras/qplanegeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype PlaneGeometryView
 * \instantiates Qt3DExtras::QPlaneGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A square planar mesh.
 */

/*!
 * \qmlproperty real PlaneGeometryView::width
 *
 * Holds the plane width.
 */

/*!
 * \qmlproperty real PlaneGeometryView::height
 *
 * Holds the plane height.
 */

/*!
 * \qmlproperty size PlaneGeometryView::meshResolution
 *
 * Holds the plane resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the mesh in the respective dimensions.
 */

/*!
 * \qmlproperty bool PlaneGeometryView::mirrored
 *
 * Controls if the UV coordinates of the plane should be flipped vertically.
 */

/*!
 * \class Qt3DExtras::QPlaneGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QPlaneGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A square planar mesh.
 */

/*!
 * Constructs a new QPlaneGeometryView with \a parent.
 */
QPlaneGeometryView::QPlaneGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QPlaneGeometry *geometry = new QPlaneGeometry(this);
    QObject::connect(geometry, &QPlaneGeometry::widthChanged, this, &QPlaneGeometryView::widthChanged);
    QObject::connect(geometry, &QPlaneGeometry::heightChanged, this, &QPlaneGeometryView::heightChanged);
    QObject::connect(geometry, &QPlaneGeometry::resolutionChanged, this, &QPlaneGeometryView::meshResolutionChanged);
    QObject::connect(geometry, &QPlaneGeometry::mirroredChanged, this, &QPlaneGeometryView::mirroredChanged);
    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QPlaneGeometryView::~QPlaneGeometryView()
{
}

void QPlaneGeometryView::setWidth(float width)
{
    static_cast<QPlaneGeometry *>(geometry())->setWidth(width);
}

/*!
 * \property QPlaneGeometryView::width
 *
 * Holds the plane width.
 */
float QPlaneGeometryView::width() const
{
    return static_cast<QPlaneGeometry *>(geometry())->width();
}

void QPlaneGeometryView::setHeight(float height)
{
    static_cast<QPlaneGeometry *>(geometry())->setHeight(height);
}

/*!
 * \property QPlaneGeometryView::height
 *
 * Holds the plane height.
 */
float QPlaneGeometryView::height() const
{
    return static_cast<QPlaneGeometry *>(geometry())->height();
}

void QPlaneGeometryView::setMeshResolution(const QSize &resolution)
{
    static_cast<QPlaneGeometry *>(geometry())->setResolution(resolution);
}

/*!
 * \property QPlaneGeometryView::meshResolution
 *
 * Holds the plane resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the mesh in the respective dimensions.
 */
QSize QPlaneGeometryView::meshResolution() const
{
    return static_cast<QPlaneGeometry *>(geometry())->resolution();
}

void QPlaneGeometryView::setMirrored(bool mirrored)
{
    static_cast<QPlaneGeometry *>(geometry())->setMirrored(mirrored);
}

/*!
 * \property QPlaneGeometryView::mirrored
 *
 * Controls if the UV coordinates of the plane should be flipped vertically.
 */
bool QPlaneGeometryView::mirrored() const
{
    return static_cast<QPlaneGeometry *>(geometry())->mirrored();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qplanegeometryview.cpp"
