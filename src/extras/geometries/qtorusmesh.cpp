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
   \ingroup qt3d-extras-geometries
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
