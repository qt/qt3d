// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qcylindermesh.h"
#include "qcylindergeometryview.h"

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <QtGui/QVector3D>

#include <qmath.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DExtras {

/*!
 * \qmltype CylinderMesh
 * \instantiates Qt3DExtras::QCylinderMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A cylindrical mesh.
 */

/*!
 * \qmlproperty int CylinderMesh::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int CylinderMesh::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty real CylinderMesh::radius
 *
 * Holds the radius of the cylinder.
 */

/*!
 * \qmlproperty real CylinderMesh::length
 *
 * Holds the length of the cylinder.
 */

/*!
 * \class Qt3DExtras::QCylinderMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QCylinderMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A cylindrical mesh.
 */

/*!
 * Constructs a new QCylinderMesh with \a parent.
 */
QCylinderMesh::QCylinderMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QCylinderGeometryView *geometry = new QCylinderGeometryView(this);
    QObject::connect(geometry, &QCylinderGeometryView::radiusChanged, this, &QCylinderMesh::radiusChanged);
    QObject::connect(geometry, &QCylinderGeometryView::ringsChanged, this, &QCylinderMesh::ringsChanged);
    QObject::connect(geometry, &QCylinderGeometryView::slicesChanged, this, &QCylinderMesh::slicesChanged);
    QObject::connect(geometry, &QCylinderGeometryView::lengthChanged, this, &QCylinderMesh::lengthChanged);

    setView(geometry);
}

/*! \internal */
QCylinderMesh::~QCylinderMesh()
{
}

void QCylinderMesh::setRings(int rings)
{
    static_cast<QCylinderGeometryView *>(view())->setRings(rings);
}

void QCylinderMesh::setSlices(int slices)
{
    static_cast<QCylinderGeometryView *>(view())->setSlices(slices);
}

void QCylinderMesh::setRadius(float radius)
{
    static_cast<QCylinderGeometryView *>(view())->setRadius(radius);
}

void QCylinderMesh::setLength(float length)
{
    static_cast<QCylinderGeometryView *>(view())->setLength(length);
}

/*!
 * \property QCylinderMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QCylinderMesh::rings() const
{
    return static_cast<QCylinderGeometryView *>(view())->rings();
}

/*!
 * \property QCylinderMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QCylinderMesh::slices() const
{
    return static_cast<QCylinderGeometryView *>(view())->slices();
}

/*!
 * \property QCylinderMesh::radius
 *
 * Holds the radius of the cylinder.
 */
float QCylinderMesh::radius() const
{
    return static_cast<QCylinderGeometryView *>(view())->radius();
}

/*!
 * \property QCylinderMesh::length
 *
 * Holds the length of the cylinder.
 */
float QCylinderMesh::length() const
{
    return static_cast<QCylinderGeometryView *>(view())->length();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qcylindermesh.cpp"
