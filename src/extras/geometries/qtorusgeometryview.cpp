// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qtorusgeometryview.h"

#include <Qt3DExtras/qtorusgeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype TorusGeometryView
 * \nativetype Qt3DExtras::QTorusGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A toroidal mesh.
 */

/*!
 * \qmlproperty int TorusGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int TorusGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real TorusGeometryView::radius
 *
 * Holds the outer radius of the torus.
 */

/*!
 * \qmlproperty real TorusGeometryView::minorRadius
 *
 * Holds the inner radius of the torus.
 */

/*!
 * \class Qt3DExtras::QTorusGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QTorusGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DCore::QGeometryView
 *
 * \brief A toroidal mesh.
 */

/*!
 * Constructs a new QTorusGeometryView with \a parent.
 */
QTorusGeometryView::QTorusGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QTorusGeometry *geometry = new QTorusGeometry(this);
    QObject::connect(geometry, &QTorusGeometry::radiusChanged, this, &QTorusGeometryView::radiusChanged);
    QObject::connect(geometry, &QTorusGeometry::ringsChanged, this, &QTorusGeometryView::ringsChanged);
    QObject::connect(geometry, &QTorusGeometry::slicesChanged, this, &QTorusGeometryView::slicesChanged);
    QObject::connect(geometry, &QTorusGeometry::minorRadiusChanged, this, &QTorusGeometryView::minorRadiusChanged);

    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QTorusGeometryView::~QTorusGeometryView()
{
}

void QTorusGeometryView::setRings(int rings)
{
    static_cast<QTorusGeometry *>(geometry())->setRings(rings);
}

void QTorusGeometryView::setSlices(int slices)
{
    static_cast<QTorusGeometry *>(geometry())->setSlices(slices);
}

void QTorusGeometryView::setRadius(float radius)
{
    static_cast<QTorusGeometry *>(geometry())->setRadius(radius);
}

void QTorusGeometryView::setMinorRadius(float minorRadius)
{
    static_cast<QTorusGeometry *>(geometry())->setMinorRadius(minorRadius);
}

/*!
 * \property Qt3DExtras::QTorusGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */
int QTorusGeometryView::rings() const
{
    return static_cast<QTorusGeometry *>(geometry())->rings();
}

/*!
 * \property Qt3DExtras::QTorusGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */
int QTorusGeometryView::slices() const
{
    return static_cast<QTorusGeometry *>(geometry())->slices();
}

/*!
 * \property Qt3DExtras::QTorusGeometryView::radius
 *
 * Holds the outer radius of the torus.
 */
float QTorusGeometryView::radius() const
{
    return static_cast<QTorusGeometry *>(geometry())->radius();
}

/*!
 * \property Qt3DExtras::QTorusGeometryView::minorRadius
 *
 * Holds the inner radius of the torus.
 */
float QTorusGeometryView::minorRadius() const
{
    return static_cast<QTorusGeometry *>(geometry())->minorRadius();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qtorusgeometryview.cpp"
