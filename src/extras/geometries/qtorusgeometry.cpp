/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtorusgeometry.h"
#include "qtorusgeometry_p.h"
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <Qt3DRender/qattribute.h>
#include <qmath.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;

namespace  Qt3DExtras {

namespace {

QByteArray createTorusVertexData(double radius, double minorRadius,
                                 int rings, int sides)
{
    const int nVerts  = sides * (rings + 1);
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal
    const quint32 elementSize = 3 + 2 + 3;
    const quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float ringFactor = (M_PI * 2) / static_cast<float>( rings );
    const float sideFactor = (M_PI * 2) / static_cast<float>( sides );

    for (int ring = 0; ring <= rings; ++ring) {
        const float u = ring * ringFactor;
        const float cu = qCos( u );
        const float su = qSin( u );

        for (int side = 0; side < sides; ++side) {
            const float v = side * sideFactor;
            const float cv = qCos( v );
            const float sv = qSin( v );
            const float r = (radius + minorRadius * cv);

            *fptr++ = r * cu;
            *fptr++ = r * su;
            *fptr++ = minorRadius * sv;

            *fptr++ = u / (M_PI * 2);
            *fptr++ = v / (M_PI * 2);

            QVector3D n(cv * cu * r, cv * su * r, sv * r);
            n.normalize();
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();
        }
    }

    return bufferBytes;
}

QByteArray createTorusIndexData(int rings, int sides)
{
    QByteArray indexBytes;
    int faces = (sides * 2) * rings; // two tris per side, for all rings
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    for (int ring = 0; ring < rings; ++ring) {
        const int ringStart = ring * sides;
        const int nextRingStart = (ring + 1) * sides;
        for (int side = 0; side < sides; ++side) {
            const int nextSide = (side + 1) % sides;
            *indexPtr++ = (ringStart + side);
            *indexPtr++ = (nextRingStart + side);
            *indexPtr++ = (nextRingStart + nextSide);
            *indexPtr++ = ringStart + side;
            *indexPtr++ = nextRingStart + nextSide;
            *indexPtr++ = (ringStart + nextSide);
        }
    }

    return indexBytes;
}

} // anonymous

class TorusVertexDataFunctor : public QBufferDataGenerator
{
public:
    TorusVertexDataFunctor(int rings, int slices, float radius, float minorRadius)
        : m_rings(rings)
        , m_sides(slices)
        , m_radius(radius)
        , m_minorRadius(minorRadius)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createTorusVertexData(m_radius, m_minorRadius, m_rings, m_sides);
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        const TorusVertexDataFunctor *otherFunctor = functor_cast<TorusVertexDataFunctor>(&other);
        if (otherFunctor != nullptr)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_sides == m_sides &&
                    otherFunctor->m_radius == m_radius &&
                    otherFunctor->m_minorRadius == m_minorRadius);
        return false;
    }

    QT3D_FUNCTOR(TorusVertexDataFunctor)

private:
    int m_rings;
    int m_sides;
    float m_radius;
    float m_minorRadius;
};

class TorusIndexDataFunctor : public QBufferDataGenerator
{
public:
    TorusIndexDataFunctor(int rings, int slices)
        : m_rings(rings)
        , m_sides(slices)
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        return createTorusIndexData(m_rings, m_sides);
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        const TorusIndexDataFunctor *otherFunctor = functor_cast<TorusIndexDataFunctor>(&other);
        if (otherFunctor != nullptr)
            return (otherFunctor->m_rings == m_rings &&
                    otherFunctor->m_sides == m_sides);
        return false;
    }

    QT3D_FUNCTOR(TorusIndexDataFunctor)

private:
    int m_rings;
    int m_sides;
};

QTorusGeometryPrivate::QTorusGeometryPrivate()
    : QGeometryPrivate()
    , m_rings(16)
    , m_slices(16)
    , m_radius(1.0f)
    , m_minorRadius(1.0f)
    , m_positionAttribute(nullptr)
    , m_normalAttribute(nullptr)
    , m_texCoordAttribute(nullptr)
    , m_indexAttribute(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
}

void QTorusGeometryPrivate::init()
{
    Q_Q(QTorusGeometry);
    m_positionAttribute = new QAttribute(q);
    m_normalAttribute = new QAttribute(q);
    m_texCoordAttribute = new QAttribute(q);
    m_indexAttribute = new QAttribute(q);
    m_vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, q);
    m_indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, q);
    // vec3 pos, vec2 tex, vec3 normal
    const quint32 elementSize = 3 + 2 + 3;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts = (m_slices + 1) * (m_rings + 1);
    const int faces = (m_slices * 2) * m_rings;

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(nVerts);

    m_texCoordAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
    m_texCoordAttribute->setDataType(QAttribute::Float);
    m_texCoordAttribute->setDataSize(2);
    m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_texCoordAttribute->setBuffer(m_vertexBuffer);
    m_texCoordAttribute->setByteStride(stride);
    m_texCoordAttribute->setByteOffset(3 * sizeof(float));
    m_texCoordAttribute->setCount(nVerts);

    m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setDataType(QAttribute::Float);
    m_normalAttribute->setDataSize(3);
    m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(stride);
    m_normalAttribute->setByteOffset(5 * sizeof(float));
    m_normalAttribute->setCount(nVerts);

    m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttribute->setDataType(QAttribute::UnsignedShort);
    m_indexAttribute->setBuffer(m_indexBuffer);

    m_indexAttribute->setCount(faces * 3);

    m_vertexBuffer->setDataGenerator(QSharedPointer<TorusVertexDataFunctor>::create(m_rings, m_slices, m_radius, m_minorRadius));
    m_indexBuffer->setDataGenerator(QSharedPointer<TorusIndexDataFunctor>::create(m_rings, m_slices));

    q->addAttribute(m_positionAttribute);
    q->addAttribute(m_texCoordAttribute);
    q->addAttribute(m_normalAttribute);
    q->addAttribute(m_indexAttribute);
}

/*!
 * \qmltype TorusGeometry
 * \instantiates Qt3DRender::QTorusGeometry
 * \inqmlmodule Qt3D.Render
 */

/*!
 * \qmlproperty int TorusGeometry::rings
 *
 * Holds the number of rings in the torus.
 */

/*!
 * \qmlproperty int TorusGeometry::slices
 *
 * Holds the number of slices in the torus.
 */

/*!
 * \qmlproperty float TorusGeometry::radius
 *
 * Holds the outer radius of the torus.
 */

/*!
 * \qmlproperty float TorusGeometry::minorRadius
 *
 * Holds the inner radius of the torus.
 */

/*!
 * \qmlproperty Attribute TorusGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */

/*!
 * \qmlproperty Attribute TorusGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */

/*!
 * \qmlproperty Attribute TorusGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */

/*!
 * \qmlproperty Attribute TorusGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */

/*!
 * \class Qt3DRender::QTorusGeometry
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DRender::QGeometry
 */

/*!
 * Constructs a new QTorusGeometry with \a parent.
 */
QTorusGeometry::QTorusGeometry(QNode *parent)
    : QGeometry(*new QTorusGeometryPrivate(), parent)
{
    Q_D(QTorusGeometry);
    d->init();
}

/*!
 * \internal
 */
QTorusGeometry::QTorusGeometry(QTorusGeometryPrivate &dd, QNode *parent)
    : QGeometry(dd, parent)
{
    Q_D(QTorusGeometry);
    d->init();
}

/*!
 * \internal
 */
QTorusGeometry::~QTorusGeometry()
{
}

/*!
 * Updates vertices based on rings and slices.
 */
void QTorusGeometry::updateVertices()
{
    Q_D(QTorusGeometry);
    const int nVerts = d->m_slices * (d->m_rings + 1);
    d->m_positionAttribute->setCount(nVerts);
    d->m_texCoordAttribute->setCount(nVerts);
    d->m_normalAttribute->setCount(nVerts);
    d->m_vertexBuffer->setDataGenerator(QSharedPointer<TorusVertexDataFunctor>::create(d->m_rings, d->m_slices, d->m_radius, d->m_minorRadius));
}

/*!
 * Updates indices based on rings and slices.
 */
void QTorusGeometry::updateIndices()
{
    Q_D(QTorusGeometry);
    const int faces = (d->m_slices * 2) * d->m_rings;
    d->m_indexAttribute->setCount(faces * 3);
    d->m_indexBuffer->setDataGenerator(QSharedPointer<TorusIndexDataFunctor>::create(d->m_rings, d->m_slices));

}

void QTorusGeometry::setRings(int rings)
{
    Q_D(QTorusGeometry);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        updateVertices();
        updateIndices();
        emit ringsChanged(rings);
    }
}

void QTorusGeometry::setSlices(int slices)
{
    Q_D(QTorusGeometry);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        updateVertices();
        updateIndices();
        emit slicesChanged(slices);
    }
}

void QTorusGeometry::setRadius(float radius)
{
    Q_D(QTorusGeometry);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        updateVertices();
        emit radiusChanged(radius);
    }
}

void QTorusGeometry::setMinorRadius(float minorRadius)
{
    Q_D(QTorusGeometry);
    if (minorRadius != d->m_minorRadius) {
        d->m_minorRadius = minorRadius;
        updateVertices();
        emit minorRadiusChanged(minorRadius);
    }
}

/*!
 * \property QTorusGeometry::rings
 *
 * Holds the number of rings in the torus.
 */
int QTorusGeometry::rings() const
{
    Q_D(const QTorusGeometry);
    return d->m_rings;
}

/*!
 * \property QTorusGeometry::slices
 *
 * Holds the number of slices in the torus.
 */
int QTorusGeometry::slices() const
{
    Q_D(const QTorusGeometry);
    return d->m_slices;
}

/*!
 * \property QTorusGeometry::radius
 *
 * Holds the outer radius of the torus.
 */
float QTorusGeometry::radius() const
{
    Q_D(const QTorusGeometry);
    return d->m_radius;
}

/*!
 * \property QTorusGeometry::minorRadius
 *
 * Holds the inner radius of the torus.
 */
float QTorusGeometry::minorRadius() const
{
    Q_D(const QTorusGeometry);
    return d->m_minorRadius;
}

/*!
 * \property QTorusGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */
QAttribute *QTorusGeometry::positionAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_positionAttribute;
}

/*!
 * \property QTorusGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */
QAttribute *QTorusGeometry::normalAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_normalAttribute;
}

/*!
 * \property QTorusGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */
QAttribute *QTorusGeometry::texCoordAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_texCoordAttribute;
}

/*!
 * \property QTorusGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */
QAttribute *QTorusGeometry::indexAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_indexAttribute;
}

} //  Qt3DExtras

QT_END_NAMESPACE
