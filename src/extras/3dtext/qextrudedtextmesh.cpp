// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "qextrudedtextmesh.h"
#include "qextrudedtextgeometry.h"

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
 * \qmltype ExtrudedTextMesh
 * \instantiates Qt3DExtras::QExtrudedTextMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A 3D extruded Text mesh.
 *
 * The origin of the mesh is the rear left end of the text's baseline.
 */

/*!
 * \qmlproperty QString Qt3D.Extras::ExtrudedTextMesh::text
 *
 * Holds the text used for the mesh.
 */

/*!
 * \qmlproperty QFont Qt3D.Extras::ExtrudedTextMesh::font
 *
 * Holds the font of the text.

 * The mesh geometry is normalized by the font's pointSize, so a larger pointSize
 * will result in smoother, rather than larger, text. pixelSize should not
 * be used.
 */

/*!
 * \qmlproperty float Qt3D.Extras::ExtrudedTextMesh::depth
 *
 * Holds the extrusion depth of the text.
 */

/*!
 * \class Qt3DExtras::QExtrudedTextMesh
 * \inheaderfile Qt3DExtras/QExtrudedTextMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A 3D extruded Text mesh.
 *
 * The origin of the mesh is the rear left end of the text's baseline.
 */

/*!
 * Constructs a new QText3DMesh with \a parent.
 */
QExtrudedTextMesh::QExtrudedTextMesh(Qt3DCore::QNode *parent)
    : QGeometryRenderer(parent)
{
    QExtrudedTextGeometry *geometry = new QExtrudedTextGeometry();
    QObject::connect(geometry, &QExtrudedTextGeometry::depthChanged, this, &QExtrudedTextMesh::depthChanged);
    QObject::connect(geometry, &QExtrudedTextGeometry::textChanged,  this, &QExtrudedTextMesh::textChanged);
    QObject::connect(geometry, &QExtrudedTextGeometry::fontChanged,  this, &QExtrudedTextMesh::fontChanged);
    QGeometryRenderer::setGeometry(geometry);
}

/*! \internal */
QExtrudedTextMesh::~QExtrudedTextMesh()
{}

void QExtrudedTextMesh::setText(const QString &text)
{
    static_cast<QExtrudedTextGeometry*>(geometry())->setText(text);
}

void QExtrudedTextMesh::setFont(const QFont &font)
{
    static_cast<QExtrudedTextGeometry*>(geometry())->setFont(font);
}

void QExtrudedTextMesh::setDepth(float depth)
{
    static_cast<QExtrudedTextGeometry*>(geometry())->setDepth(depth);
}

/*!
 * \property QExtrudedTextMesh::text
 *
 * Holds the text used for the mesh.
 */
QString QExtrudedTextMesh::text() const
{
    return static_cast<QExtrudedTextGeometry*>(geometry())->text();
}

/*!
 * \property QExtrudedTextMesh::font
 *
 * Holds the font of the text.
 *
 * The mesh geometry is normalized by the font's pointSize, so a larger pointSize
 * will result in smoother, rather than larger, text. pixelSize should not
 * be used.
 */
QFont QExtrudedTextMesh::font() const
{
    return static_cast<QExtrudedTextGeometry*>(geometry())->font();
}

/*!
 * \property QExtrudedTextMesh::depth
 *
 * Holds the extrusion depth of the text.
 */
float QExtrudedTextMesh::depth() const
{
    return static_cast<QExtrudedTextGeometry*>(geometry())->extrusionLength();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qextrudedtextmesh.cpp"
