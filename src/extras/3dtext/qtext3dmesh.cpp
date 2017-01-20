/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtext3dmesh.h"
#include "qtext3dgeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
 * \qmltype Text3DMesh
 * \instantiates Qt3DExtras::QText3DMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A 3D Text mesh.
 */

/*!
 * \qmlproperty QString Text3DMesh::text
 *
 * Holds the text used for the mesh.
 */

/*!
 * \qmlproperty QFont Text3DMesh::font
 *
 * Holds the font of the text.
 */

/*!
 * \qmlproperty float Text3DMesh::depth
 *
 * Holds the extrusion depth of the text.
 */

/*!
 * \qmlproperty float Text3DMesh::edgeSplitAngle
 *
 * Holds the threshold angle for smooth normals.
 */

/*!
 * \class Qt3DExtras::QText3DMesh
 * \inheaderfile Qt3DExtras/QText3DMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A 3D Text mesh.
 */

/*!
 * Constructs a new QText3DMesh with \a parent.
 */
QText3DMesh::QText3DMesh(Qt3DCore::QNode *parent)
    : QGeometryRenderer(parent)
{
    QText3DGeometry *geometry = new QText3DGeometry();
    QObject::connect(geometry, &QText3DGeometry::depthChanged, this, &QText3DMesh::depthChanged);
    QObject::connect(geometry, &QText3DGeometry::textChanged,  this, &QText3DMesh::textChanged);
    QObject::connect(geometry, &QText3DGeometry::fontChanged,  this, &QText3DMesh::fontChanged);
    QObject::connect(geometry, &QText3DGeometry::edgeSplitAngleChanged,  this, &QText3DMesh::edgeSplitAngleChanged);
    QGeometryRenderer::setGeometry(geometry);
}

/*! \internal */
QText3DMesh::~QText3DMesh()
{}

void QText3DMesh::setText(QString text)
{
    static_cast<QText3DGeometry*>(geometry())->setText(text);
}

void QText3DMesh::setFont(QFont font)
{
    static_cast<QText3DGeometry*>(geometry())->setFont(font);
}

void QText3DMesh::setDepth(float depth)
{
    static_cast<QText3DGeometry*>(geometry())->setDepth(depth);
}

void QText3DMesh::setEdgeSplitAngle(float smoothAngle)
{
    static_cast<QText3DGeometry*>(geometry())->setEdgeSplitAngle(smoothAngle);
}

/*!
 * \property QString QText3DMesh::text
 *
 * Holds the text used for the mesh.
 */
QString QText3DMesh::text()
{
    return static_cast<QText3DGeometry*>(geometry())->text();
}

/*!
 * \property QFont QText3DMesh::font
 *
 * Holds the font of the text.
 */
QFont QText3DMesh::font()
{
    return static_cast<QText3DGeometry*>(geometry())->font();
}

/*!
 * \property float QText3DMesh::depth
 *
 * Holds the extrusion depth of the text.
 */
float QText3DMesh::depth()
{
    return static_cast<QText3DGeometry*>(geometry())->depth();
}

/*!
 * \property float QText3DMesh::edgeSplitAngle
 *
 * Holds the threshold angle for smooth normals.
 */
float QText3DMesh::edgeSplitAngle()
{
    return static_cast<QText3DGeometry*>(geometry())->edgeSplitAngle();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
