// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcuboidgeometry.h"
#include "qcuboidgeometry_p.h"

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE


namespace  Qt3DExtras {

using namespace Qt3DCore;

namespace {

enum PlaneNormal {
    PositiveX,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ
};

void createPlaneVertexData(float w, float h, const QSize &resolution,
                           PlaneNormal normal, float planeDistance,
                           float *vertices)
{
    const float a0 = -w / 2.0f;
    const float b0 = -h / 2.0f;
    const float da = w / (resolution.width() - 1);
    const float db = h / (resolution.height() - 1);
    const float du = 1.0f / (resolution.width() - 1);
    const float dv = 1.0f / (resolution.height() - 1);

    switch (normal) {
    case NegativeX:
        // Iterate over z
        for (int j = resolution.height() - 1; j >= 0; --j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over y
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = planeDistance;
                *vertices++ = a;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = v;
                *vertices++ = u;

                // normal
                *vertices++ = -1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
                *vertices++ = 1.0f;
            }
        }
        break;

    case PositiveX: {
        // Iterate over z
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over y
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = planeDistance;
                *vertices++ = a;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = 1.0f - v;
                *vertices++ = u;

                // normal
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = -1.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }

    case NegativeY:
        // Iterate over z
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            // This iterates in the opposite sense to the other directions
            // so that the winding order is correct
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = planeDistance;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = -1.0f;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;

    case PositiveY: {
        // Iterate over z
        for (int j = resolution.height() - 1; j >= 0; --j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            // This iterates in the opposite sense to the other directions
            // so that the winding order is correct
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = planeDistance;
                *vertices++ = b;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = 1.0f - v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;

                // tangent
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }

    case NegativeZ:
        // Iterate over y
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            for (int i = resolution.width() - 1; i >= 0; --i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = b;
                *vertices++ = planeDistance;

                // texture coordinates
                *vertices++ = 1.0f - u;
                *vertices++ = v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = -1.0f;

                // tangent
                *vertices++ = -1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;

    case PositiveZ: {
        // Iterate over y
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            for (int i = 0; i < resolution.width(); ++i) {
                const float a = a0 + static_cast<float>(i) * da;
                const float u = static_cast<float>(i) * du;

                // position
                *vertices++ = a;
                *vertices++ = b;
                *vertices++ = planeDistance;

                // texture coordinates
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;

                // tangent
                *vertices++ = 1.0f;
                *vertices++ = 0.0f;
                *vertices++ = 0.0f;
                *vertices++ = 1.0f;
            }
        }
        break;
    }
    } // switch (normal)
}

void createPlaneIndexData(const QSize &resolution, quint16 *indices, quint16 &baseVertex)
{
    // Populate indices taking care to get correct CCW winding on all faces
    // Iterate over v direction (rows)
    for (int j = 0; j < resolution.height() - 1; ++j) {
        const int rowStartIndex = j * resolution.width() + baseVertex;
        const int nextRowStartIndex = (j + 1) * resolution.width() + baseVertex;

        // Iterate over u direction (columns)
        for (int i = 0; i < resolution.width() - 1; ++i) {
            // Split quad into two triangles
            *indices++ = rowStartIndex + i;
            *indices++ = rowStartIndex + i + 1;
            *indices++ = nextRowStartIndex + i;

            *indices++ = nextRowStartIndex + i;
            *indices++ = rowStartIndex + i + 1;
            *indices++ = nextRowStartIndex + i + 1;
        }
    }
    baseVertex += resolution.width() * resolution.height();
}

QByteArray createCuboidVertexData(float xExtent,
                                  float yExtent,
                                  float zExtent,
                                  const QSize &yzResolution,
                                  const QSize &xzResolution,
                                  const QSize &xyResolution)
{
    Q_ASSERT(xExtent > 0.0f && yExtent > 0.0f && zExtent > 0.0);
    Q_ASSERT(yzResolution.width() >= 2 && yzResolution.height() >=2);
    Q_ASSERT(xzResolution.width() >= 2 && xzResolution.height() >=2);
    Q_ASSERT(xyResolution.width() >= 2 && xyResolution.height() >=2);

    const int yzVerts = yzResolution.width() * yzResolution.height();
    const int xzVerts = xzResolution.width() * xzResolution.height();
    const int xyVerts = xyResolution.width() * xyResolution.height();
    const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);

    const quint32 elementSize = 3 + 3 + 2 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray vertexBytes;
    vertexBytes.resize(stride * nVerts);
    float* vertices = reinterpret_cast<float*>(vertexBytes.data());

    createPlaneVertexData(yExtent, zExtent, yzResolution, PositiveX, xExtent * 0.5f, vertices);
    vertices += yzVerts * elementSize;
    createPlaneVertexData(yExtent, zExtent, yzResolution, NegativeX, -xExtent * 0.5f, vertices);
    vertices += yzVerts * elementSize;
    createPlaneVertexData(xExtent, zExtent, xzResolution, PositiveY, yExtent * 0.5f, vertices);
    vertices += xzVerts * elementSize;
    createPlaneVertexData(xExtent, zExtent, xzResolution, NegativeY, -yExtent * 0.5f, vertices);
    vertices += xzVerts * elementSize;
    createPlaneVertexData(xExtent, yExtent, xyResolution, PositiveZ, zExtent * 0.5f, vertices);
    vertices += xyVerts * elementSize;
    createPlaneVertexData(xExtent, yExtent, xyResolution, NegativeZ, -zExtent * 0.5f, vertices);

    return vertexBytes;
}

QByteArray createCuboidIndexData(const QSize &yzResolution,
                                 const QSize &xzResolution,
                                 const QSize &xyResolution)
{
    Q_ASSERT(yzResolution.width() >= 2 && yzResolution.height() >= 2);
    Q_ASSERT(xzResolution.width() >= 2 && xzResolution.height() >= 2);
    Q_ASSERT(xyResolution.width() >= 2 && xyResolution.height() >= 2);

    const int yzIndices = 2 * 3 * (yzResolution.width() - 1) * (yzResolution.height() - 1);
    const int xzIndices = 2 * 3 * (xzResolution.width() - 1) * (xzResolution.height() - 1);
    const int xyIndices = 2 * 3 * (xyResolution.width() - 1) * (xyResolution.height() - 1);
    const qsizetype indexCount = 2 * (yzIndices + xzIndices + xyIndices);

    QByteArray indexData;
    indexData.resize(indexCount * sizeof(quint16));
    quint16 *indices = reinterpret_cast<quint16 *>(indexData.data());
    quint16 baseIndex = 0;

    createPlaneIndexData(yzResolution, indices, baseIndex);
    indices += yzIndices;
    createPlaneIndexData(yzResolution, indices, baseIndex);
    indices += yzIndices;
    createPlaneIndexData(xzResolution, indices, baseIndex);
    indices += xzIndices;
    createPlaneIndexData(xzResolution, indices, baseIndex);
    indices += xzIndices;
    createPlaneIndexData(xyResolution, indices, baseIndex);
    indices += xyIndices;
    createPlaneIndexData(xyResolution, indices, baseIndex);

    return indexData;
}

} // anonymous

QCuboidGeometryPrivate::QCuboidGeometryPrivate()
    : QGeometryPrivate()
    , m_xExtent(1.0f)
    , m_yExtent(1.0f)
    , m_zExtent(1.0f)
    , m_yzFaceResolution(2, 2)
    , m_xzFaceResolution(2, 2)
    , m_xyFaceResolution(2, 2)
    , m_positionAttribute(nullptr)
    , m_normalAttribute(nullptr)
    , m_texCoordAttribute(nullptr)
    , m_tangentAttribute(nullptr)
    , m_indexAttribute(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
}

void QCuboidGeometryPrivate::init()
{
    Q_Q(QCuboidGeometry);
    m_positionAttribute = new Qt3DCore::QAttribute(q);
    m_normalAttribute = new Qt3DCore::QAttribute(q);
    m_texCoordAttribute = new Qt3DCore::QAttribute(q);
    m_tangentAttribute = new Qt3DCore::QAttribute(q);
    m_indexAttribute = new Qt3DCore::QAttribute(q);
    m_vertexBuffer = new Qt3DCore::QBuffer(q);
    m_indexBuffer = new Qt3DCore::QBuffer(q);

    // vec3 pos vec2 tex vec3 normal vec4 tangent
    const quint32 stride = (3 + 2 + 3 + 4) * sizeof(float);
    const int yzIndices = 2 * 3 * (m_yzFaceResolution.width() - 1) * (m_yzFaceResolution.height() - 1);
    const int xzIndices = 2 * 3 * (m_xzFaceResolution.width() - 1) * (m_xzFaceResolution.height() - 1);
    const int xyIndices = 2 * 3 * (m_xyFaceResolution.width() - 1) * (m_xyFaceResolution.height() - 1);
    const int yzVerts = m_yzFaceResolution.width() * m_yzFaceResolution.height();
    const int xzVerts = m_xzFaceResolution.width() * m_xzFaceResolution.height();
    const int xyVerts = m_xyFaceResolution.width() * m_xyFaceResolution.height();

    const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);
    const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);

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

    m_indexAttribute->setCount(indexCount);

    m_vertexBuffer->setData(generateVertexData());
    m_indexBuffer->setData(generateIndexData());

    q->addAttribute(m_positionAttribute);
    q->addAttribute(m_texCoordAttribute);
    q->addAttribute(m_normalAttribute);
    q->addAttribute(m_tangentAttribute);
    q->addAttribute(m_indexAttribute);
}

QByteArray QCuboidGeometryPrivate::generateVertexData() const
{
    return createCuboidVertexData(m_xExtent, m_yExtent, m_zExtent,
                                  m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution);
}

QByteArray QCuboidGeometryPrivate::generateIndexData() const
{
    return createCuboidIndexData(m_yzFaceResolution, m_xzFaceResolution, m_xyFaceResolution);
}

/*!
 * \qmltype CuboidGeometry
 * \nativetype Qt3DExtras::QCuboidGeometry
 * \inqmlmodule Qt3D.Extras
 * \brief CuboidGeometry allows creation of a cuboid in 3D space.
 *
 * The CuboidGeometry type is most commonly used internally by the CuboidMesh type
 * but can also be used in custom GeometryRenderer types.
 */

/*!
 * \qmlproperty real CuboidGeometry::xExtent
 *
 * Holds the x extent of the geometry.
 */

/*!
 * \qmlproperty real CuboidGeometry::yExtent
 *
 * Holds the y extent of the geometry.
 */

/*!
 * \qmlproperty real CuboidGeometry::zExtent
 *
 * Holds the z extent of the geometry.
 */

/*!
 * \qmlproperty size CuboidGeometry::yzMeshResolution
 *
 * Holds the y-z resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidGeometry::xzMeshResolution
 *
 * Holds the x-z resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */

/*!
 * \qmlproperty size CuboidGeometry::xyMeshResolution
 *
 * Holds the x-y resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */

/*!
 * \qmlproperty Attribute CuboidGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */

/*!
 * \qmlproperty Attribute CuboidGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */

/*!
 * \qmlproperty Attribute CuboidGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */

/*!
 * \qmlproperty Attribute CuboidGeometry::tangentAttribute
 *
 * Holds the geometry tangent attribute.
 */

/*!
 * \qmlproperty Attribute CuboidGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */

/*!
 * \class Qt3DExtras::QCuboidGeometry
 * \ingroup qt3d-extras-geometries
 * \inheaderfile Qt3DExtras/QCuboidGeometry
 * \inmodule Qt3DExtras
 * \brief The QCuboidGeometry class allows creation of a cuboid in 3D space.
 * \since 5.7
 * \ingroup geometries
 * \inherits Qt3DCore::QGeometry
 *
 * The QCuboidGeometry class is most commonly used internally by the QCuboidMesh
 * but can also be used in custom Qt3DRender::QGeometryRenderer subclasses.
 */

/*!
 * Constructs a new QCuboidGeometry with \a parent.
 */
QCuboidGeometry::QCuboidGeometry(QNode *parent)
    : QGeometry(*new QCuboidGeometryPrivate(), parent)
{
    Q_D(QCuboidGeometry);
    d->init();
}

/*!
 * \internal
 */
QCuboidGeometry::QCuboidGeometry(QCuboidGeometryPrivate &dd, QNode *parent)
    : QGeometry(dd, parent)
{
    Q_D(QCuboidGeometry);
    d->init();
}

/*!
 * \internal
 */
QCuboidGeometry::~QCuboidGeometry()
{
}

/*!
 * Updates indices based on mesh resolutions.
 */
void QCuboidGeometry::updateIndices()
{
    Q_D(QCuboidGeometry);
    const int yzIndices = 2 * 3 * (d->m_yzFaceResolution.width() - 1) * (d->m_yzFaceResolution.height() - 1);
    const int xzIndices = 2 * 3 * (d->m_xzFaceResolution.width() - 1) * (d->m_xzFaceResolution.height() - 1);
    const int xyIndices = 2 * 3 * (d->m_xyFaceResolution.width() - 1) * (d->m_xyFaceResolution.height() - 1);
    const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);

    d->m_indexAttribute->setCount(indexCount);
    d->m_indexBuffer->setData(d->generateIndexData());
}

/*!
 * Updates vertices based on mesh resolutions.
 */
void QCuboidGeometry::updateVertices()
{
    Q_D(QCuboidGeometry);
    const int yzVerts = d->m_yzFaceResolution.width() * d->m_yzFaceResolution.height();
    const int xzVerts = d->m_xzFaceResolution.width() * d->m_xzFaceResolution.height();
    const int xyVerts = d->m_xyFaceResolution.width() * d->m_xyFaceResolution.height();
    const int nVerts = 2 * (yzVerts + xzVerts + xyVerts);

    d->m_positionAttribute->setCount(nVerts);
    d->m_normalAttribute->setCount(nVerts);
    d->m_texCoordAttribute->setCount(nVerts);
    d->m_tangentAttribute->setCount(nVerts);

    d->m_vertexBuffer->setData(d->generateVertexData());
}

void QCuboidGeometry::setXExtent(float xExtent)
{
    Q_D(QCuboidGeometry);
    if (d->m_xExtent != xExtent) {
        d->m_xExtent = xExtent;
        updateVertices();
        emit xExtentChanged(xExtent);
    }
}

void QCuboidGeometry::setYExtent(float yExtent)
{
    Q_D(QCuboidGeometry);
    if (d->m_yExtent != yExtent) {
        d->m_yExtent = yExtent;
        updateVertices();
        emit yExtentChanged(yExtent);
    }
}

void QCuboidGeometry::setZExtent(float zExtent)
{
    Q_D(QCuboidGeometry);
    if (d->m_zExtent != zExtent) {
        d->m_zExtent = zExtent;
        updateVertices();
        emit zExtentChanged(zExtent);
    }
}

void QCuboidGeometry::setYZMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidGeometry);
    if (d->m_yzFaceResolution != resolution) {
        d->m_yzFaceResolution = resolution;
        updateVertices();
        updateIndices();
        emit yzMeshResolutionChanged(resolution);
    }
}

void QCuboidGeometry::setXZMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidGeometry);
    if (d->m_xzFaceResolution != resolution) {
        d->m_xzFaceResolution = resolution;
        updateVertices();
        updateIndices();
        emit xzMeshResolutionChanged(resolution);
    }
}

void QCuboidGeometry::setXYMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidGeometry);
    if (d->m_xyFaceResolution != resolution) {
        d->m_xyFaceResolution = resolution;
        updateVertices();
        updateIndices();
        emit xyMeshResolutionChanged(resolution);
    }
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::xExtent
 *
 * Holds the x extent of the geometry.
 */
float QCuboidGeometry::xExtent() const
{
    Q_D(const QCuboidGeometry);
    return d->m_xExtent;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::yExtent
 *
 * Holds the y extent of the geometry.
 */
float QCuboidGeometry::yExtent() const
{
    Q_D(const QCuboidGeometry);
    return d->m_yExtent;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::zExtent
 *
 * Holds the z extent of the geometry.
 */
float QCuboidGeometry::zExtent() const
{
    Q_D(const QCuboidGeometry);
    return d->m_zExtent;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::yzMeshResolution
 *
 * Holds the y-z resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the y-z faces of the mesh.
 */
QSize QCuboidGeometry::yzMeshResolution() const
{
    Q_D(const QCuboidGeometry);
    return d->m_yzFaceResolution;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::xzMeshResolution
 *
 * Holds the x-z resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the x-z faces of the mesh.
 */
QSize QCuboidGeometry::xyMeshResolution() const
{
    Q_D(const QCuboidGeometry);
    return d->m_xyFaceResolution;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::xyMeshResolution
 *
 * Holds the x-y resolution.
 * The width and height values of this property specify the number of vertices generated for
 * the x-y faces of the mesh.
 */
QSize QCuboidGeometry::xzMeshResolution() const
{
    Q_D(const QCuboidGeometry);
    return d->m_xzFaceResolution;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::positionAttribute
 *
 * Holds the geometry position attribute.
 */
QAttribute *QCuboidGeometry::positionAttribute() const
{
    Q_D(const QCuboidGeometry);
    return d->m_positionAttribute;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::normalAttribute
 *
 * Holds the geometry normal attribute.
 */
QAttribute *QCuboidGeometry::normalAttribute() const
{
    Q_D(const QCuboidGeometry);
    return d->m_normalAttribute;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::texCoordAttribute
 *
 * Holds the geometry texture coordinate attribute.
 */
QAttribute *QCuboidGeometry::texCoordAttribute() const
{
    Q_D(const QCuboidGeometry);
    return d->m_texCoordAttribute;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::tangentAttribute
 *
 * Holds the geometry tangent attribute.
 */
QAttribute *QCuboidGeometry::tangentAttribute() const
{
    Q_D(const QCuboidGeometry);
    return d->m_tangentAttribute;
}

/*!
 * \property Qt3DExtras::QCuboidGeometry::indexAttribute
 *
 * Holds the geometry index attribute.
 */
QAttribute *QCuboidGeometry::indexAttribute() const
{
    Q_D(const QCuboidGeometry);
    return d->m_indexAttribute;
}

} //  Qt3DExtras

QT_END_NAMESPACE

#include "moc_qcuboidgeometry.cpp"
