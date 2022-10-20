/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
