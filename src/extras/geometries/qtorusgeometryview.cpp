/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2022 The Qt Company Ltd and/or its subsidiary(-ies).
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
******************************************************************************/

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qtorusgeometryview.h"

#include <Qt3DExtras/qtorusgeometry.h>

QT_BEGIN_NAMESPACE

namespace  Qt3DExtras {

/*!
 * \qmltype TorusGeometryView
 * \instantiates Qt3DExtras::QTorusGeometryView
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
 * \property QTorusGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */
int QTorusGeometryView::rings() const
{
    return static_cast<QTorusGeometry *>(geometry())->rings();
}

/*!
 * \property QTorusGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */
int QTorusGeometryView::slices() const
{
    return static_cast<QTorusGeometry *>(geometry())->slices();
}

/*!
 * \property QTorusGeometryView::radius
 *
 * Holds the outer radius of the torus.
 */
float QTorusGeometryView::radius() const
{
    return static_cast<QTorusGeometry *>(geometry())->radius();
}

/*!
 * \property QTorusGeometryView::minorRadius
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
