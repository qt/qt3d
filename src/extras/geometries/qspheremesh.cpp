/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qspheremesh.h"

#include <Qt3DExtras/qspheregeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype SphereMesh
 * \instantiates Qt3DExtras::QSphereMesh
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
   \ingroup qt3d-extras-geometries
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
    : QGeometryRenderer(parent)
{
    QSphereGeometry *geometry = new QSphereGeometry(this);
    QObject::connect(geometry, &QSphereGeometry::radiusChanged, this, &QSphereMesh::radiusChanged);
    QObject::connect(geometry, &QSphereGeometry::ringsChanged, this, &QSphereMesh::ringsChanged);
    QObject::connect(geometry, &QSphereGeometry::slicesChanged, this, &QSphereMesh::slicesChanged);
    QObject::connect(geometry, &QSphereGeometry::generateTangentsChanged, this, &QSphereMesh::generateTangentsChanged);
    QGeometryRenderer::setGeometry(geometry);
}

/*! \internal */
QSphereMesh::~QSphereMesh()
{
}

void QSphereMesh::setRings(int rings)
{
    static_cast<QSphereGeometry *>(geometry())->setRings(rings);
}

void QSphereMesh::setSlices(int slices)
{
    static_cast<QSphereGeometry *>(geometry())->setSlices(slices);
}

void QSphereMesh::setRadius(float radius)
{
    static_cast<QSphereGeometry *>(geometry())->setRadius(radius);
}

void QSphereMesh::setGenerateTangents(bool gen)
{
    static_cast<QSphereGeometry *>(geometry())->setGenerateTangents(gen);
}

/*!
 * \property QSphereMesh::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */
bool QSphereMesh::generateTangents() const
{
    return static_cast<QSphereGeometry *>(geometry())->generateTangents();
}

/*!
 * \property QSphereMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QSphereMesh::rings() const
{
    return static_cast<QSphereGeometry *>(geometry())->rings();
}

/*!
 * \property QSphereMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QSphereMesh::slices() const
{
    return static_cast<QSphereGeometry *>(geometry())->slices();
}

/*!
 * \property QSphereMesh::radius
 *
 * Holds the radius of the sphere.
 */
float QSphereMesh::radius() const
{
    return static_cast<QSphereGeometry *>(geometry())->radius();
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qspheremesh.cpp"
