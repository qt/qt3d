/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qconegeometryview.h"

#include <Qt3DExtras/qconegeometry.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <QtGui/QVector3D>

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
 * \qmltype ConeGeometryView
 * \instantiates Qt3DExtras::QConeGeometryView
 * \inqmlmodule Qt3D.Extras
 * \brief A conical mesh.
 */

/*!
 * \qmlproperty int ConeGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int ConeGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty bool ConeGeometryView::hasTopEndcap
 *
 * Determines if the cone top is capped or open.
 */

/*!
 * \qmlproperty bool ConeGeometryView::hasBottomEndcap
 *
 * Determines if the cone bottom is capped or open.
 */

/*!
 * \qmlproperty real ConeGeometryView::topRadius
 *
 * Holds the top radius of the cone.
 */

/*!
 * \qmlproperty real ConeGeometryView::bottomRadius
 *
 * Holds the bottom radius of the cone.
 */

/*!
 * \qmlproperty real ConeGeometryView::length
 *
 * Holds the length of the cone.
 */

/*!
 * \class Qt3DExtras::QConeGeometryView
   \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QConeGeometryView
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A conical mesh.
 */

QConeGeometryView::QConeGeometryView(QNode *parent)
    : Qt3DCore::QGeometryView(parent)
{
    QConeGeometry *geometry = new QConeGeometry(this);
    QObject::connect(geometry, &QConeGeometry::hasTopEndcapChanged, this, &QConeGeometryView::hasTopEndcapChanged);
    QObject::connect(geometry, &QConeGeometry::hasBottomEndcapChanged, this, &QConeGeometryView::hasBottomEndcapChanged);
    QObject::connect(geometry, &QConeGeometry::topRadiusChanged, this, &QConeGeometryView::topRadiusChanged);
    QObject::connect(geometry, &QConeGeometry::bottomRadiusChanged, this, &QConeGeometryView::bottomRadiusChanged);
    QObject::connect(geometry, &QConeGeometry::ringsChanged, this, &QConeGeometryView::ringsChanged);
    QObject::connect(geometry, &QConeGeometry::slicesChanged, this, &QConeGeometryView::slicesChanged);
    QObject::connect(geometry, &QConeGeometry::lengthChanged, this, &QConeGeometryView::lengthChanged);

    QGeometryView::setGeometry(geometry);
}

/*! \internal */
QConeGeometryView::~QConeGeometryView()
{
}

void QConeGeometryView::setHasTopEndcap(bool hasTopEndcap)
{
    static_cast<QConeGeometry *>(geometry())->setHasTopEndcap(hasTopEndcap);
}

void QConeGeometryView::setHasBottomEndcap(bool hasBottomEndcap)
{
    static_cast<QConeGeometry *>(geometry())->setHasBottomEndcap(hasBottomEndcap);
}

void QConeGeometryView::setTopRadius(float topRadius)
{
    static_cast<QConeGeometry *>(geometry())->setTopRadius(topRadius);
}

void QConeGeometryView::setBottomRadius(float bottomRadius)
{
    static_cast<QConeGeometry *>(geometry())->setBottomRadius(bottomRadius);
}

void QConeGeometryView::setRings(int rings)
{
    static_cast<QConeGeometry *>(geometry())->setRings(rings);
}

void QConeGeometryView::setSlices(int slices)
{
    static_cast<QConeGeometry *>(geometry())->setSlices(slices);
}

void QConeGeometryView::setLength(float length)
{
    static_cast<QConeGeometry *>(geometry())->setLength(length);
}

/*!
 * \property QConeGeometryView::hasTopEndcap
 *
 * Determines if the cone top is capped or open.
 */
bool QConeGeometryView::hasTopEndcap() const
{
    return static_cast<QConeGeometry *>(geometry())->hasTopEndcap();
}

/*!
 * \property QConeGeometryView::hasBottomEndcap
 *
 * Determines if the cone bottom is capped or open.
 */
bool QConeGeometryView::hasBottomEndcap() const
{
    return static_cast<QConeGeometry *>(geometry())->hasBottomEndcap();
}

/*!
 * \property QConeGeometryView::topRadius
 *
 * Holds the top radius of the cone.
 */
float QConeGeometryView::topRadius() const
{
    return static_cast<QConeGeometry *>(geometry())->topRadius();
}

/*!
 * \property QConeGeometryView::bottomRadius
 *
 * Holds the bottom radius of the cone.
 */
float QConeGeometryView::bottomRadius() const
{
    return static_cast<QConeGeometry *>(geometry())->bottomRadius();
}

/*!
 * \property QConeGeometryView::rings
 *
 * Holds the number of rings in the mesh.
 */
int QConeGeometryView::rings() const
{
    return static_cast<QConeGeometry *>(geometry())->rings();
}

/*!
 * \property QConeGeometryView::slices
 *
 * Holds the number of slices in the mesh.
 */
int QConeGeometryView::slices() const
{
    return static_cast<QConeGeometry *>(geometry())->slices();
}

/*!
 * \property QConeGeometryView::length
 *
 * Holds the length of the cone.
 */
float QConeGeometryView::length() const
{
    return static_cast<QConeGeometry *>(geometry())->length();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
