// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qcylindergeometryview.h"

#include <Qt3DExtras/qcylindergeometry.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <QtGui/QVector3D>

#include <qmath.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DExtras {

/*!
 * \qmltype CylinderGeometryView
 * \instantiates Qt3DExtras::QCylinderGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A cylindrical mesh.
 */

/*!
 * \qmlproperty int CylinderGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int CylinderGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real CylinderGeometryView::radius
 *
 * Holds the radius of the cylinder.
 */

/*!
 * \qmlproperty real CylinderGeometryView::length
 *
 * Holds the length of the cylinder.
 */

/*!
 * \class Qt3DExtras::QCylinderGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QCylinderGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A cylindrical mesh.
 */

/*!
 * Constructs a new QCylinderGeometryView with \a parent.
 */
QCylinderGeometryView::QCylinderGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QCylinderGeometry *geometry = new QCylinderGeometry(this);
    QObject::connect(geometry, &QCylinderGeometry::radiusChanged, this, &QCylinderGeometryView::radiusChanged);
    QObject::connect(geometry, &QCylinderGeometry::ringsChanged, this, &QCylinderGeometryView::ringsChanged);
    QObject::connect(geometry, &QCylinderGeometry::slicesChanged, this, &QCylinderGeometryView::slicesChanged);
    QObject::connect(geometry, &QCylinderGeometry::lengthChanged, this, &QCylinderGeometryView::lengthChanged);

    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QCylinderGeometryView::~QCylinderGeometryView()
{
}

void QCylinderGeometryView::setRings(int rings)
{
    static_cast<QCylinderGeometry *>(geometry())->setRings(rings);
}

void QCylinderGeometryView::setSlices(int slices)
{
    static_cast<QCylinderGeometry *>(geometry())->setSlices(slices);
}

void QCylinderGeometryView::setRadius(float radius)
{
    static_cast<QCylinderGeometry *>(geometry())->setRadius(radius);
}

void QCylinderGeometryView::setLength(float length)
{
    static_cast<QCylinderGeometry *>(geometry())->setLength(length);
}

/*!
 * \property QCylinderGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */
int QCylinderGeometryView::rings() const
{
    return static_cast<QCylinderGeometry *>(geometry())->rings();
}

/*!
 * \property QCylinderGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */
int QCylinderGeometryView::slices() const
{
    return static_cast<QCylinderGeometry *>(geometry())->slices();
}

/*!
 * \property QCylinderGeometryView::radius
 *
 * Holds the radius of the cylinder.
 */
float QCylinderGeometryView::radius() const
{
    return static_cast<QCylinderGeometry *>(geometry())->radius();
}

/*!
 * \property QCylinderGeometryView::length
 *
 * Holds the length of the cylinder.
 */
float QCylinderGeometryView::length() const
{
    return static_cast<QCylinderGeometry *>(geometry())->length();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qcylindergeometryview.cpp"
