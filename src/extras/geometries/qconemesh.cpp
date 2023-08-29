// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qconemesh.h"

#include <Qt3DExtras/qconegeometryview.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <QtGui/QVector3D>

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
 * \qmltype ConeMesh
 * \instantiates Qt3DExtras::QConeMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A conical mesh.
 */

/*!
 * \qmlproperty int ConeMesh::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int ConeMesh::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty bool ConeMesh::hasTopEndcap
 *
 * Determines if the cone top is capped or open.
 */

/*!
 * \qmlproperty bool ConeMesh::hasBottomEndcap
 *
 * Determines if the cone bottom is capped or open.
 */

/*!
 * \qmlproperty real ConeMesh::topRadius
 *
 * Holds the top radius of the cone.
 */

/*!
 * \qmlproperty real ConeMesh::bottomRadius
 *
 * Holds the bottom radius of the cone.
 */

/*!
 * \qmlproperty real ConeMesh::length
 *
 * Holds the length of the cone.
 */

/*!
 * \class Qt3DExtras::QConeMesh
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QConeMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A conical mesh.
 */

QConeMesh::QConeMesh(QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    QConeGeometryView *geometry = new QConeGeometryView(this);
    QObject::connect(geometry, &QConeGeometryView::hasTopEndcapChanged, this, &QConeMesh::hasTopEndcapChanged);
    QObject::connect(geometry, &QConeGeometryView::hasBottomEndcapChanged, this, &QConeMesh::hasBottomEndcapChanged);
    QObject::connect(geometry, &QConeGeometryView::topRadiusChanged, this, &QConeMesh::topRadiusChanged);
    QObject::connect(geometry, &QConeGeometryView::bottomRadiusChanged, this, &QConeMesh::bottomRadiusChanged);
    QObject::connect(geometry, &QConeGeometryView::ringsChanged, this, &QConeMesh::ringsChanged);
    QObject::connect(geometry, &QConeGeometryView::slicesChanged, this, &QConeMesh::slicesChanged);
    QObject::connect(geometry, &QConeGeometryView::lengthChanged, this, &QConeMesh::lengthChanged);

    setView(geometry);
}

/*! \internal */
QConeMesh::~QConeMesh()
{
}

void QConeMesh::setHasTopEndcap(bool hasTopEndcap)
{
    static_cast<QConeGeometryView *>(view())->setHasTopEndcap(hasTopEndcap);
}

void QConeMesh::setHasBottomEndcap(bool hasBottomEndcap)
{
    static_cast<QConeGeometryView *>(view())->setHasBottomEndcap(hasBottomEndcap);
}

void QConeMesh::setTopRadius(float topRadius)
{
    static_cast<QConeGeometryView *>(view())->setTopRadius(topRadius);
}

void QConeMesh::setBottomRadius(float bottomRadius)
{
    static_cast<QConeGeometryView *>(view())->setBottomRadius(bottomRadius);
}

void QConeMesh::setRings(int rings)
{
    static_cast<QConeGeometryView *>(view())->setRings(rings);
}

void QConeMesh::setSlices(int slices)
{
    static_cast<QConeGeometryView *>(view())->setSlices(slices);
}

void QConeMesh::setLength(float length)
{
    static_cast<QConeGeometryView *>(view())->setLength(length);
}

/*!
 * \property QConeMesh::hasTopEndcap
 *
 * Determines if the cone top is capped or open.
 */
bool QConeMesh::hasTopEndcap() const
{
    return static_cast<QConeGeometryView *>(view())->hasTopEndcap();
}

/*!
 * \property QConeMesh::hasBottomEndcap
 *
 * Determines if the cone bottom is capped or open.
 */
bool QConeMesh::hasBottomEndcap() const
{
    return static_cast<QConeGeometryView *>(view())->hasBottomEndcap();
}

/*!
 * \property QConeMesh::topRadius
 *
 * Holds the top radius of the cone.
 */
float QConeMesh::topRadius() const
{
    return static_cast<QConeGeometryView *>(view())->topRadius();
}

/*!
 * \property QConeMesh::bottomRadius
 *
 * Holds the bottom radius of the cone.
 */
float QConeMesh::bottomRadius() const
{
    return static_cast<QConeGeometryView *>(view())->bottomRadius();
}

/*!
 * \property QConeMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QConeMesh::rings() const
{
    return static_cast<QConeGeometryView *>(view())->rings();
}

/*!
 * \property QConeMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QConeMesh::slices() const
{
    return static_cast<QConeGeometryView *>(view())->slices();
}

/*!
 * \property QConeMesh::length
 *
 * Holds the length of the cone.
 */
float QConeMesh::length() const
{
    return static_cast<QConeGeometryView *>(view())->length();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qconemesh.cpp"
