// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qspheremesh.h"

#include <Qt3DExtras/qspheregeometryview.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype SphereMesh
 * \nativetype Qt3DExtras::QSphereMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A spherical mesh.
 */

/*!
 * \qmlproperty int SphereMesh::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int SphereMesh::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real SphereMesh::radius
 *
 * Holds the radius of the sphere.
 */

/*!
 * \qmlproperty bool SphereMesh::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */

/*!
 * \class Qt3DExtras::QSphereMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QSphereMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A spherical mesh.
 */

/*!
 * Constructs a new QSphereMesh with \a parent.
 */
QSphereMesh::QSphereMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QSphereGeometryView *geometry = new QSphereGeometryView(this);
    QObject::connect(geometry, &QSphereGeometryView::radiusChanged, this, &QSphereMesh::radiusChanged);
    QObject::connect(geometry, &QSphereGeometryView::ringsChanged, this, &QSphereMesh::ringsChanged);
    QObject::connect(geometry, &QSphereGeometryView::slicesChanged, this, &QSphereMesh::slicesChanged);
    QObject::connect(geometry, &QSphereGeometryView::generateTangentsChanged, this, &QSphereMesh::generateTangentsChanged);
    setView(geometry);
}

/*! \internal */
QSphereMesh::~QSphereMesh()
{
}

void QSphereMesh::setRings(int rings)
{
    static_cast<QSphereGeometryView *>(view())->setRings(rings);
}

void QSphereMesh::setSlices(int slices)
{
    static_cast<QSphereGeometryView *>(view())->setSlices(slices);
}

void QSphereMesh::setRadius(float radius)
{
    static_cast<QSphereGeometryView *>(view())->setRadius(radius);
}

void QSphereMesh::setGenerateTangents(bool gen)
{
    static_cast<QSphereGeometryView *>(view())->setGenerateTangents(gen);
}

/*!
 * \property Qt3DExtras::QSphereMesh::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */
bool QSphereMesh::generateTangents() const
{
    return static_cast<QSphereGeometryView *>(view())->generateTangents();
}

/*!
 * \property Qt3DExtras::QSphereMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QSphereMesh::rings() const
{
    return static_cast<QSphereGeometryView *>(view())->rings();
}

/*!
 * \property Qt3DExtras::QSphereMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QSphereMesh::slices() const
{
    return static_cast<QSphereGeometryView *>(view())->slices();
}

/*!
 * \property Qt3DExtras::QSphereMesh::radius
 *
 * Holds the radius of the sphere.
 */
float QSphereMesh::radius() const
{
    return static_cast<QSphereGeometryView *>(view())->radius();
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qspheremesh.cpp"
