// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qspheregeometryview.h"

#include <Qt3DExtras/qspheregeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype SphereGeometryView
 * \instantiates Qt3DExtras::QSphereGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A spherical mesh.
 */

/*!
 * \qmlproperty int SphereGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int SphereGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real SphereGeometryView::radius
 *
 * Holds the radius of the sphere.
 */

/*!
 * \qmlproperty bool SphereGeometryView::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */

/*!
 * \class Qt3DExtras::QSphereGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QSphereGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DCore::QGeometryView
 *
 * \brief A spherical mesh.
 */

/*!
 * Constructs a new QSphereGeometryView with \a parent.
 */
QSphereGeometryView::QSphereGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QSphereGeometry *geometry = new QSphereGeometry(this);
    QObject::connect(geometry, &QSphereGeometry::radiusChanged, this, &QSphereGeometryView::radiusChanged);
    QObject::connect(geometry, &QSphereGeometry::ringsChanged, this, &QSphereGeometryView::ringsChanged);
    QObject::connect(geometry, &QSphereGeometry::slicesChanged, this, &QSphereGeometryView::slicesChanged);
    QObject::connect(geometry, &QSphereGeometry::generateTangentsChanged, this, &QSphereGeometryView::generateTangentsChanged);
    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QSphereGeometryView::~QSphereGeometryView()
{
}

void QSphereGeometryView::setRings(int rings)
{
    static_cast<QSphereGeometry *>(geometry())->setRings(rings);
}

void QSphereGeometryView::setSlices(int slices)
{
    static_cast<QSphereGeometry *>(geometry())->setSlices(slices);
}

void QSphereGeometryView::setRadius(float radius)
{
    static_cast<QSphereGeometry *>(geometry())->setRadius(radius);
}

void QSphereGeometryView::setGenerateTangents(bool gen)
{
    static_cast<QSphereGeometry *>(geometry())->setGenerateTangents(gen);
}

/*!
 * \property QSphereGeometryView::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */
bool QSphereGeometryView::generateTangents() const
{
    return static_cast<QSphereGeometry *>(geometry())->generateTangents();
}

/*!
 * \property QSphereGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */
int QSphereGeometryView::rings() const
{
    return static_cast<QSphereGeometry *>(geometry())->rings();
}

/*!
 * \property QSphereGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */
int QSphereGeometryView::slices() const
{
    return static_cast<QSphereGeometry *>(geometry())->slices();
}

/*!
 * \property QSphereGeometryView::radius
 *
 * Holds the radius of the sphere.
 */
float QSphereGeometryView::radius() const
{
    return static_cast<QSphereGeometry *>(geometry())->radius();
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qspheregeometryview.cpp"
