// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtorusgeometry.h"
#include "qtorusgeometry_p.h"

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

#include <qmath.h>

QT_BEGIN_NAMESPACE


namespace  Qt3DExtras {

using namespace Qt3DCore;

namespace {

int vertexCount(int requestedRings, int requestedSlices)
{
    return (requestedRings + 1) * (requestedSlices + 1);
}

int triangleCount(int requestedRings, int requestedSlices)
{
    return 2 * requestedRings * requestedSlices;
}

QByteArray createTorusVertexData(double radius, double minorRadius,
                                 int rings, int slices)
{
    // The additional side and ring compared to what the user asked
    // for is because we also need to ensure proper texture coordinate
    // wrapping at the seams. Without this the last ring and side would
    // interpolate the texture coordinates from ~0.9x back down to 0
    // (the starting value at the first ring). So we insert an extra
    // ring and side with the same positions as the first ring and side
    // but with texture coordinates of 1 (compared to 0).
    const int nVerts  = vertexCount(rings, slices);
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float ringFactor = (M_PI * 2) / static_cast<float>(rings);
    const float sliceFactor = (M_PI * 2) / static_cast<float>(slices);

    for (int ring = 0; ring <= rings; ++ring) {
        const float u = ring * ringFactor;
        const float cu = qCos(u);
        const float su = qSin(u);

        for (int slice = 0; slice <= slices; ++slice) {
            const float v = slice * sliceFactor;
            const float cv = qCos(v + M_PI);
            const float sv = qSin(v);
            const float r = (radius + minorRadius * cv);

            *fptr++ = r * cu;
            *fptr++ = r * su;
            *fptr++ = minorRadius * sv;

            *fptr++ = u / (M_PI * 2);
            *fptr++ = v / (M_PI * 2);

            QVector3D n(cv * cu, cv * su, sv);
            n.normalize();
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();

            QVector4D t(-su, cu, 0.0f, 1.0f);
            t.normalize();
            *fptr++ = t.x();
            *fptr++ = t.y();
            *fptr++ = t.z();
            *fptr++ = t.w();
        }
    }

    return bufferBytes;
}

QByteArray createTorusIndexData(int requestedRings, int requestedSlices)
{
    const int slices = requestedSlices + 1;
    int triangles = triangleCount(requestedRings, requestedSlices);
    qsizetype indices = triangles * 3;
    Q_ASSERT(indices < 65536);
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    for (int ring = 0; ring < requestedRings; ++ring) {
        const int ringStart = ring * slices;
        const int nextRingStart = (ring + 1) * slices;
        for (int slice = 0; slice < requestedSlices; ++slice) {
            const int nextSlice = (slice + 1) % slices;
            *indexPtr++ = ringStart + slice;
            *indexPtr++ = ringStart + nextSlice;
            *indexPtr++ = nextRingStart + slice;
            *indexPtr++ = ringStart + nextSlice;
            *indexPtr++ = nextRingStart + nextSlice;
            *indexPtr++ = nextRingStart + slice;
        }
    }

    return indexBytes;
}

} // anonymous

QTorusGeometryPrivate::QTorusGeometryPrivate()
    : QGeometryPrivate()
    , m_rings(16)
    , m_slices(16)
    , m_radius(1.0f)
    , m_minorRadius(1.0f)
    , m_positionAttribute(nullptr)
    , m_normalAttribute(nullptr)
    , m_texCoordAttribute(nullptr)
    , m_tangentAttribute(nullptr)
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
    m_tangentAttribute = new QAttribute(q);
    m_indexAttribute = new QAttribute(q);
    m_vertexBuffer = new Qt3DCore::QBuffer(q);
    m_indexBuffer = new Qt3DCore::QBuffer(q);
    // vec3 pos, vec2 tex, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    const int nVerts = vertexCount(m_rings, m_slices);
    const int triangles = triangleCount(m_rings, m_slices);

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

    m_indexAttribute->setCount(triangles * 3);

    m_vertexBuffer->setData(generateVertexData());
    m_indexBuffer->setData(generateIndexData());

    q->addAttribute(m_positionAttribute);
    q->addAttribute(m_texCoordAttribute);
    q->addAttribute(m_normalAttribute);
    q->addAttribute(m_tangentAttribute);
    q->addAttribute(m_indexAttribute);
}

QByteArray QTorusGeometryPrivate::generateVertexData() const
{
    return createTorusVertexData(m_radius, m_minorRadius, m_rings, m_slices);
}

QByteArray QTorusGeometryPrivate::generateIndexData() const
{
    return createTorusIndexData(m_rings, m_slices);
}

/*!
 * \qmltype TorusGeometry
 * \nativetype Qt3DExtras::QTorusGeometry
 * \inqmlmodule Qt3D.Extras
 * \brief TorusGeometry allows creation of a torus in 3D space.
 *
 * The TorusGeometry type is most commonly used internally by the TorusMesh type
 * but can also be used in custom GeometryRenderer types.
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
 * \qmlproperty real TorusGeometry::radius
 *
 * Holds the outer radius of the torus.
 */

/*!
 * \qmlproperty real TorusGeometry::minorRadius
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
 * \class Qt3DExtras::QTorusGeometry
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QTorusGeometry
 * \inmodule Qt3DExtras
 * \brief The QTorusGeometry class allows creation of a torus in 3D space.
 * \since 5.7
 * \ingroup geometries
 * \inherits Qt3DCore::QGeometry
 *
 * The QTorusGeometry class is most commonly used internally by the QTorusMesh
 * but can also be used in custom Qt3DRender::QGeometryRenderer subclasses.
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
 * Updates vertices based on rings, slices, and radius properties.
 */
void QTorusGeometry::updateVertices()
{
    Q_D(QTorusGeometry);
    const int nVerts = vertexCount(d->m_rings, d->m_slices);
    d->m_positionAttribute->setCount(nVerts);
    d->m_texCoordAttribute->setCount(nVerts);
    d->m_normalAttribute->setCount(nVerts);
    d->m_vertexBuffer->setData(d->generateVertexData());
}

/*!
 * Updates indices based on rings and slices properties.
 */
void QTorusGeometry::updateIndices()
{
    Q_D(QTorusGeometry);
    const int triangles = triangleCount(d->m_rings, d->m_slices);
    d->m_indexAttribute->setCount(triangles * 3);
    d->m_indexBuffer->setData(d->generateIndexData());
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
 * \property Qt3DExtras::QTorusGeometry::rings
 *
 * Holds the number of rings in the torus.
 */
int QTorusGeometry::rings() const
{
    Q_D(const QTorusGeometry);
    return d->m_rings;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::slices
 *
 * Holds the number of slices in the torus.
 */
int QTorusGeometry::slices() const
{
    Q_D(const QTorusGeometry);
    return d->m_slices;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::radius
 *
 * Holds the outer radius of the torus.
 */
float QTorusGeometry::radius() const
{
    Q_D(const QTorusGeometry);
    return d->m_radius;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::minorRadius
 *
 * Holds the inner radius of the torus.
 */
float QTorusGeometry::minorRadius() const
{
    Q_D(const QTorusGeometry);
    return d->m_minorRadius;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */
QAttribute *QTorusGeometry::positionAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_positionAttribute;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */
QAttribute *QTorusGeometry::normalAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_normalAttribute;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */
QAttribute *QTorusGeometry::texCoordAttribute() const
{
    Q_D(const QTorusGeometry);
    return d->m_texCoordAttribute;
}

/*!
 * \property Qt3DExtras::QTorusGeometry::indexAttribute
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

#include "moc_qtorusgeometry.cpp"
