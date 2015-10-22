/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qspheremesh.h"
#include "qspheregeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

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

QSphereMesh::~QSphereMesh()
{
    QNode::cleanup();
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

bool QSphereMesh::generateTangents() const
{
    return static_cast<QSphereGeometry *>(geometry())->generateTangents();
}

int QSphereMesh::rings() const
{
    return static_cast<QSphereGeometry *>(geometry())->rings();
}

int QSphereMesh::slices() const
{
    return static_cast<QSphereGeometry *>(geometry())->slices();
}

float QSphereMesh::radius() const
{
    return static_cast<QSphereGeometry *>(geometry())->radius();
}

} //Qt3D

QT_END_NAMESPACE
