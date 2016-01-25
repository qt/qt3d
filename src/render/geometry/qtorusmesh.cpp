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
#include "qtorusgeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QTorusMesh::QTorusMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    QTorusGeometry *geometry = new QTorusGeometry(this);
    QObject::connect(geometry, &QTorusGeometry::radiusChanged, this, &QTorusMesh::radiusChanged);
    QObject::connect(geometry, &QTorusGeometry::ringsChanged, this, &QTorusMesh::ringsChanged);
    QObject::connect(geometry, &QTorusGeometry::slicesChanged, this, &QTorusMesh::slicesChanged);
    QObject::connect(geometry, &QTorusGeometry::minorRadiusChanged, this, &QTorusMesh::minorRadiusChanged);

    QGeometryRenderer::setGeometry(geometry);
}

QTorusMesh::~QTorusMesh()
{
    QGeometryRenderer::cleanup();
}

void QTorusMesh::setRings(int rings)
{
    static_cast<QTorusGeometry *>(geometry())->setRings(rings);
}

void QTorusMesh::setSlices(int slices)
{
    static_cast<QTorusGeometry *>(geometry())->setSlices(slices);
}

void QTorusMesh::setRadius(float radius)
{
    static_cast<QTorusGeometry *>(geometry())->setRadius(radius);
}

void QTorusMesh::setMinorRadius(float minorRadius)
{
    static_cast<QTorusGeometry *>(geometry())->setMinorRadius(minorRadius);
}

int QTorusMesh::rings() const
{
    return static_cast<QTorusGeometry *>(geometry())->rings();
}

int QTorusMesh::slices() const
{
    return static_cast<QTorusGeometry *>(geometry())->slices();
}

float QTorusMesh::radius() const
{
    return static_cast<QTorusGeometry *>(geometry())->radius();
}

float QTorusMesh::minorRadius() const
{
    return static_cast<QTorusGeometry *>(geometry())->minorRadius();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
