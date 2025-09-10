// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qspheregeometry.h"
#include "qspheregeometry_p.h"

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>

#include <qmath.h>

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif


QT_BEGIN_NAMESPACE


namespace  Qt3DExtras {

using namespace Qt3DCore;

namespace {

QByteArray createSphereMeshVertexData(float radius, int rings, int slices)
{
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts  = (slices + 1) * (rings + 1);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float dTheta = (M_PI * 2) / static_cast<float>( slices );
    const float dPhi = M_PI / static_cast<float>( rings );
    const float du = 1.0f / static_cast<float>( slices );
    const float dv = 1.0f / static_cast<float>( rings );

    // Iterate over latitudes (rings)
    for ( int lat = 0; lat < rings + 1; ++lat )
    {
        const float phi = M_PI_2 - static_cast<float>( lat ) * dPhi;
        const float cosPhi = qCos( phi );
        const float sinPhi = qSin( phi );
        const float v = 1.0f - static_cast<float>( lat ) * dv;

        // Iterate over longitudes (slices)
        for ( int lon = 0; lon < slices + 1; ++lon )
        {
            const float theta = static_cast<float>( lon ) * dTheta;
            const float cosTheta = qCos( theta );
            const float sinTheta = qSin( theta );
            const float u = static_cast<float>( lon ) * du;

            *fptr++ = radius * cosTheta * cosPhi;
            *fptr++ = radius * sinPhi;
            *fptr++ = radius * sinTheta * cosPhi;

            *fptr++ = u;
            *fptr++ = v;

            *fptr++   = cosTheta * cosPhi;
            *fptr++ = sinPhi;
            *fptr++ = sinTheta * cosPhi;

            *fptr++ = sinTheta;
            *fptr++ = 0.0;
            *fptr++ = -cosTheta;
            *fptr++ = 1.0;
        }
    }
    return bufferBytes;
}

QByteArray createSphereMeshIndexData(int rings, int slices)
{
    int faces = (slices * 2) * (rings - 2); // two tris per slice, for all middle rings
    faces += 2 * slices; // tri per slice for both top and bottom

    QByteArray indexBytes;
    const qsizetype indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16 *indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    // top cap
    {
        const int nextRingStartIndex = slices + 1;
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = 0;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    for ( int i = 1; i < (rings - 1); ++i )
    {
        const int ringStartIndex = i * ( slices + 1 );
        const int nextRingStartIndex = ( i + 1 ) * ( slices + 1 );

        for ( int j = 0; j < slices; ++j )
        {
            // Split the quad into two triangles
            *indexPtr++ = ringStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    // bottom cap
    {
        const int ringStartIndex = (rings - 1) * ( slices + 1);
        const int nextRingStartIndex = (rings) * ( slices + 1);
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex;
            *indexPtr++ = ringStartIndex + j;
        }
    }

    return indexBytes;
}

} // anonymous

QSphereGeometryPrivate::QSphereGeometryPrivate()
    : QGeometryPrivate()
    , m_generateTangents(false)
    , m_rings(16)
    , m_slices(16)
    , m_radius(1.0f)
    , m_positionAttribute(nullptr)
    , m_normalAttribute(nullptr)
    , m_texCoordAttribute(nullptr)
    , m_tangentAttribute(nullptr)
    , m_indexAttribute(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
}

void QSphereGeometryPrivate::init()
{
    Q_Q(QSphereGeometry);
    m_positionAttribute = new QAttribute(q);
    m_normalAttribute = new QAttribute(q);
    m_texCoordAttribute = new QAttribute(q);
    m_tangentAttribute = new QAttribute(q);
    m_indexAttribute = new QAttribute(q);
    m_vertexBuffer = new Qt3DCore::QBuffer(q);
    m_indexBuffer = new Qt3DCore::QBuffer(q);

    // vec3 pos, vec2 tex, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts = (m_slices + 1) * (m_rings + 1);
    const int faces = (m_slices * 2) * (m_rings - 2) + (2 * m_slices);

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(QAttribute::Float);
    m_positionAttribute->setVertexSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(nVerts);

    m_texCoordAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
    m_texCoordAttribute->setVertexBaseType(QAttribute::Float);
    m_texCoordAttribute->setVertexSize(2);
    m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_texCoordAttribute->setBuffer(m_vertexBuffer);
    m_texCoordAttribute->setByteStride(stride);
    m_texCoordAttribute->setByteOffset(3 * sizeof(float));
    m_texCoordAttribute->setCount(nVerts);

    m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setVertexBaseType(QAttribute::Float);
    m_normalAttribute->setVertexSize(3);
    m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(stride);
    m_normalAttribute->setByteOffset(5 * sizeof(float));
    m_normalAttribute->setCount(nVerts);

    m_tangentAttribute->setName(QAttribute::defaultTangentAttributeName());
    m_tangentAttribute->setVertexBaseType(QAttribute::Float);
    m_tangentAttribute->setVertexSize(4);
    m_tangentAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_tangentAttribute->setBuffer(m_vertexBuffer);
    m_tangentAttribute->setByteStride(stride);
    m_tangentAttribute->setByteOffset(8 * sizeof(float));
    m_tangentAttribute->setCount(nVerts);

    m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
    m_indexAttribute->setBuffer(m_indexBuffer);

    m_indexAttribute->setCount(faces * 3);

    m_vertexBuffer->setData(generateVertexData());
    m_indexBuffer->setData(generateIndexData());

    q->addAttribute(m_positionAttribute);
    q->addAttribute(m_texCoordAttribute);
    q->addAttribute(m_normalAttribute);
    if (m_generateTangents)
        q->addAttribute(m_tangentAttribute);
    q->addAttribute(m_indexAttribute);
}

QByteArray QSphereGeometryPrivate::generateVertexData() const
{
    return createSphereMeshVertexData(m_radius, m_rings, m_slices);
}

QByteArray QSphereGeometryPrivate::generateIndexData() const
{
    return createSphereMeshIndexData(m_rings, m_slices);
}

/*!
 * \qmltype SphereGeometry
 * \nativetype Qt3DExtras::QSphereGeometry
 * \inqmlmodule Qt3D.Extras
 * \brief SphereGeometry allows creation of a sphere in 3D space.
 *
 * The SphereGeometry type is most commonly used internally by the SphereMesh type
 * but can also be used in custom GeometryRenderer types.
 */

/*!
 * \qmlproperty int SphereGeometry::rings
 *
 * Holds the number of rings in the sphere.
 */

/*!
 * \qmlproperty int SphereGeometry::slices
 *
 * Holds the number of slices in the sphere.
 */

/*!
 * \qmlproperty real SphereGeometry::radius
 *
 * Holds the radius of the sphere.
 */

/*!
 * \qmlproperty bool SphereGeometry::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */

/*!
 * \qmlproperty Attribute SphereGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */

/*!
 * \qmlproperty Attribute SphereGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */

/*!
 * \qmlproperty Attribute SphereGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */

/*!
 * \qmlproperty Attribute SphereGeometry::tangentAttribute
 *
 * Holds the geometry tangent attribute.
 */

/*!
 * \qmlproperty Attribute SphereGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */

/*!
 * \class Qt3DExtras::QSphereGeometry
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QSphereGeometry
 * \inmodule Qt3DExtras
 * \brief The QSphereGeometry class allows creation of a sphere in 3D space.
 * \since 5.7
 * \ingroup geometries
 * \inherits Qt3DCore::QGeometry
 *
 * The QSphereGeometry class is most commonly used internally by the QSphereMesh
 * but can also be used in custom Qt3DRender::QGeometryRenderer subclasses.
 */

/*!
 * Constructs a new QSphereGeometry with \a parent.
 */
QSphereGeometry::QSphereGeometry(QNode *parent)
    : QGeometry(*new QSphereGeometryPrivate(), parent)
{
    Q_D(QSphereGeometry);
    d->init();
}

/*!
 * \internal
 */
QSphereGeometry::QSphereGeometry(QSphereGeometryPrivate &dd, QNode *parent)
    : QGeometry(dd, parent)
{
    Q_D(QSphereGeometry);
    d->init();
}

/*!
 * \internal
 */
QSphereGeometry::~QSphereGeometry()
{
}

/*!
 * Updates vertices based on rings, slices, and radius properties
 */
void QSphereGeometry::updateVertices()
{
    Q_D(QSphereGeometry);
    const int nVerts = (d->m_slices + 1) * (d->m_rings + 1);
    d->m_positionAttribute->setCount(nVerts);
    d->m_texCoordAttribute->setCount(nVerts);
    d->m_normalAttribute->setCount(nVerts);
    d->m_tangentAttribute->setCount(nVerts);
    d->m_vertexBuffer->setData(d->generateVertexData());
}

/*!
 * Updates indices based on rings and slices properties.
 */
void QSphereGeometry::updateIndices()
{
    Q_D(QSphereGeometry);
    const int faces = (d->m_slices * 2) * (d->m_rings - 2) + (2 * d->m_slices);
    d->m_indexAttribute->setCount(faces * 3);
    d->m_indexBuffer->setData(d->generateIndexData());
}

void QSphereGeometry::setRings(int rings)
{
    Q_D(QSphereGeometry);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        updateVertices();
        updateIndices();
        emit ringsChanged(rings);
    }
}

void QSphereGeometry::setSlices(int slices)
{
    Q_D(QSphereGeometry);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        updateVertices();
        updateIndices();
        emit slicesChanged(slices);
    }
}

void QSphereGeometry::setRadius(float radius)
{
    Q_D(QSphereGeometry);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        updateVertices();
        emit radiusChanged(radius);
    }
}

void QSphereGeometry::setGenerateTangents(bool gen)
{
    Q_D(QSphereGeometry);
    if (d->m_generateTangents != gen) {
        if (d->m_generateTangents)
            removeAttribute(d->m_tangentAttribute);
        d->m_generateTangents = gen;
        if (d->m_generateTangents)
            addAttribute(d->m_tangentAttribute);
        emit generateTangentsChanged(gen);
    }
}

/*!
 * \property Qt3DExtras::QSphereGeometry::generateTangents
 *
 * Holds the value of the automatic tangent vectors generation flag.
 * Tangent vectors are orthogonal to normal vectors.
 */
bool QSphereGeometry::generateTangents() const
{
    Q_D(const QSphereGeometry);
    return d->m_generateTangents;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::rings
 *
 * Holds the number of rings in the sphere.
 */
int QSphereGeometry::rings() const
{
    Q_D(const QSphereGeometry);
    return d->m_rings;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::slices
 *
 * Holds the number of slices in the sphere.
 */
int QSphereGeometry::slices() const
{
    Q_D(const QSphereGeometry);
    return d->m_slices;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::radius
 *
 * Holds the radius of the sphere.
 */
float QSphereGeometry::radius() const
{
    Q_D(const QSphereGeometry);
    return d->m_radius;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */
QAttribute *QSphereGeometry::positionAttribute() const
{
    Q_D(const QSphereGeometry);
    return d->m_positionAttribute;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */
QAttribute *QSphereGeometry::normalAttribute() const
{
    Q_D(const QSphereGeometry);
    return d->m_normalAttribute;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */
QAttribute *QSphereGeometry::texCoordAttribute() const
{
    Q_D(const QSphereGeometry);
    return d->m_texCoordAttribute;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::tangentAttribute
 *
 * Holds the geometry tangent attribute.
 */
QAttribute *QSphereGeometry::tangentAttribute() const
{
    Q_D(const QSphereGeometry);
    return d->m_tangentAttribute;
}

/*!
 * \property Qt3DExtras::QSphereGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */
QAttribute *QSphereGeometry::indexAttribute() const
{
    Q_D(const QSphereGeometry);
    return d->m_indexAttribute;
}

} //  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qspheregeometry.cpp"

