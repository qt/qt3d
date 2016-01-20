/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplanemesh.h"
#include "qplanegeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \qmltype PlaneMesh
 * \instantiates Qt3DRender::QPlaneMesh
 * \inqmlmodule Qt3D.Render
 * \brief A square planar mesh.
 */

/*!
 * \qmlproperty float PlaneMesh::width
 *
 * Holds the plane width.
 */

/*!
 * \qmlproperty float PlaneMesh::height
 *
 * Holds the plane height.
 */

/*!
 * \qmlproperty size PlaneMesh::meshResolution
 *
 * Holds the plane resolution.
 */

/*!
 * \class Qt3DRender::QPlaneMesh
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A square planar mesh.
 */

/*!
 * Constructs a new QPlaneMesh with \a parent.
 */
QPlaneMesh::QPlaneMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    QPlaneGeometry *geometry = new QPlaneGeometry(this);
    QObject::connect(geometry, &QPlaneGeometry::widthChanged, this, &QPlaneMesh::widthChanged);
    QObject::connect(geometry, &QPlaneGeometry::heightChanged, this, &QPlaneMesh::heightChanged);
    QObject::connect(geometry, &QPlaneGeometry::resolutionChanged, this, &QPlaneMesh::meshResolutionChanged);
    QGeometryRenderer::setGeometry(geometry);
}

/*!
 * Destroys this plane mesh.
 */
QPlaneMesh::~QPlaneMesh()
{
    QNode::cleanup();
}

void QPlaneMesh::setWidth(float width)
{
    static_cast<QPlaneGeometry *>(geometry())->setWidth(width);
}

/*!
 * \property QPlaneMesh::width
 *
 * Holds the plane width.
 */
float QPlaneMesh::width() const
{
    return static_cast<QPlaneGeometry *>(geometry())->width();
}

void QPlaneMesh::setHeight(float height)
{
    static_cast<QPlaneGeometry *>(geometry())->setHeight(height);
}

/*!
 * \property QPlaneMesh::height
 *
 * Holds the plane height.
 */
float QPlaneMesh::height() const
{
    return static_cast<QPlaneGeometry *>(geometry())->height();
}

void QPlaneMesh::setMeshResolution(const QSize &resolution)
{
    static_cast<QPlaneGeometry *>(geometry())->setResolution(resolution);
}

/*!
 * \property QPlaneMesh::meshResolution
 *
 * Holds the plane resolution.
 */
QSize QPlaneMesh::meshResolution() const
{
    return static_cast<QPlaneGeometry *>(geometry())->resolution();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
