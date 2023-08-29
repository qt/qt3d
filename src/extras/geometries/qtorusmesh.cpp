// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qtorusmesh.h"
#include "qtorusgeometryview.h"

#include <Qt3DExtras/qtorusgeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype TorusMesh
 * \instantiates Qt3DExtras::QTorusMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A toroidal mesh.
 */

/*!
 * \qmlproperty int TorusMesh::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int TorusMesh::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real TorusMesh::radius
 *
 * Holds the outer radius of the torus.
 */

/*!
 * \qmlproperty real TorusMesh::minorRadius
 *
 * Holds the inner radius of the torus.
 */

/*!
 * \class Qt3DExtras::QTorusMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QTorusMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A toroidal mesh.
 */

/*!
 * Constructs a new QTorusMesh with \a parent.
 */
QTorusMesh::QTorusMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QTorusGeometryView *geometry = new QTorusGeometryView(this);
    QObject::connect(geometry, &QTorusGeometryView::radiusChanged, this, &QTorusMesh::radiusChanged);
    QObject::connect(geometry, &QTorusGeometryView::ringsChanged, this, &QTorusMesh::ringsChanged);
    QObject::connect(geometry, &QTorusGeometryView::slicesChanged, this, &QTorusMesh::slicesChanged);
    QObject::connect(geometry, &QTorusGeometryView::minorRadiusChanged, this, &QTorusMesh::minorRadiusChanged);

    setView(geometry);
}

/*! \internal */
QTorusMesh::~QTorusMesh()
{
}

void QTorusMesh::setRings(int rings)
{
    static_cast<QTorusGeometryView *>(view())->setRings(rings);
}

void QTorusMesh::setSlices(int slices)
{
    static_cast<QTorusGeometryView *>(view())->setSlices(slices);
}

void QTorusMesh::setRadius(float radius)
{
    static_cast<QTorusGeometryView *>(view())->setRadius(radius);
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    static_cast<QTorusGeometryView *>(view())->setMinorRadius(minorRadius);
}

/*!
 * \property QTorusMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QTorusMesh::rings() const
{
    return static_cast<QTorusGeometryView *>(view())->rings();
}

/*!
 * \property QTorusMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QTorusMesh::slices() const
{
    return static_cast<QTorusGeometryView *>(view())->slices();
}

/*!
 * \property QTorusMesh::radius
 *
 * Holds the outer radius of the torus.
 */
float QTorusMesh::radius() const
{
    return static_cast<QTorusGeometryView *>(view())->radius();
}

/*!
 * \property QTorusMesh::minorRadius
 *
 * Holds the inner radius of the torus.
 */
float QTorusMesh::minorRadius() const
{
    return static_cast<QTorusGeometryView *>(view())->minorRadius();
}

} // namespace  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qtorusmesh.cpp"
