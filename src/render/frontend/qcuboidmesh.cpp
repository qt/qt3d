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

#include "qcuboidmesh.h"
#include "qcuboidmesh_p.h"

#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

#include <limits>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QCuboidMeshPrivate
    \internal
*/
QCuboidMeshPrivate::QCuboidMeshPrivate()
    : QAbstractMeshPrivate()
    , m_xExtent(1.0)
    , m_yExtent(1.0)
    , m_zExtent(1.0)
    , m_yzFaceResolution(2, 2)
    , m_xzFaceResolution(2, 2)
    , m_xyFaceResolution(2, 2)
{
}

void QCuboidMesh::copy(const QNode *ref)
{
    QAbstractMesh::copy(ref);
    const QCuboidMesh *mesh = static_cast<const QCuboidMesh*>(ref);
    d_func()->m_xExtent = mesh->d_func()->m_xExtent;
    d_func()->m_yExtent = mesh->d_func()->m_yExtent;
    d_func()->m_zExtent = mesh->d_func()->m_zExtent;
    d_func()->m_yzFaceResolution = mesh->d_func()->m_yzFaceResolution;
    d_func()->m_xzFaceResolution = mesh->d_func()->m_xzFaceResolution;
    d_func()->m_xyFaceResolution = mesh->d_func()->m_xyFaceResolution;
}

QCuboidMesh::QCuboidMesh(QNode *parent)
    : QAbstractMesh(*new QCuboidMeshPrivate, parent)
{
    update();
}

/*! \internal */
QCuboidMesh::QCuboidMesh(QCuboidMeshPrivate &dd, QNode *parent)
    : QAbstractMesh(dd, parent)
{
    update();
}

void QCuboidMesh::setXExtent(float xExtent)
{
    Q_D(QCuboidMesh);
    if (d->m_xExtent != xExtent) {
        d->m_xExtent = xExtent;
        emit xExtentChanged();
        update();
    }
}

float QCuboidMesh::xExtent() const
{
    Q_D(const QCuboidMesh);
    return d->m_xExtent;
}

void QCuboidMesh::setYExtent(float yExtent)
{
    Q_D(QCuboidMesh);
    if (d->m_yExtent != yExtent) {
        d->m_yExtent = yExtent;
        emit yExtentChanged();
        update();
    }
}

float QCuboidMesh::yExtent() const
{
    Q_D(const QCuboidMesh);
    return d->m_yExtent;
}

void QCuboidMesh::setZExtent(float zExtent)
{
    Q_D(QCuboidMesh);
    if (d->m_zExtent != zExtent) {
        d->m_zExtent = zExtent;
        emit zExtentChanged();
        update();
    }
}

float QCuboidMesh::zExtent() const
{
    Q_D(const QCuboidMesh);
    return d->m_zExtent;
}

void QCuboidMesh::setYZMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidMesh);
    if (d->m_yzFaceResolution != resolution) {
        d->m_yzFaceResolution = resolution;
        emit yzMeshResolutionChanged();
        update();
    }
}

QSize QCuboidMesh::yzMeshResolution() const
{
    Q_D(const QCuboidMesh);
    return d->m_yzFaceResolution;
}

void QCuboidMesh::setXZMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidMesh);
    if (d->m_xzFaceResolution != resolution) {
        d->m_xzFaceResolution = resolution;
        emit xzMeshResolutionChanged();
        update();
    }
}

QSize QCuboidMesh::xzMeshResolution() const
{
    Q_D(const QCuboidMesh);
    return d->m_xzFaceResolution;
}

void QCuboidMesh::setXYMeshResolution(const QSize &resolution)
{
    Q_D(QCuboidMesh);
    if (d->m_xyFaceResolution != resolution) {
        d->m_xyFaceResolution = resolution;
        emit xyMeshResolutionChanged();
        update();
    }
}

QSize QCuboidMesh::xyMeshResolution() const
{
    Q_D(const QCuboidMesh);
    return d->m_xyFaceResolution;
}

QMeshDataPtr createCuboidMesh(float xExtent,
                              float yExtent,
                              float zExtent,
                              const QSize &yzResolution,
                              const QSize &xzResolution,
                              const QSize &xyResolution);

class CuboidMeshFunctor : public QAbstractMeshFunctor
{
public:
    CuboidMeshFunctor(const QCuboidMesh &c)
        : QAbstractMeshFunctor()
        , m_xExtent(c.xExtent())
        , m_yExtent(c.yExtent())
        , m_zExtent(c.zExtent())
        , m_yzFaceResolution(c.yzMeshResolution())
        , m_xzFaceResolution(c.xzMeshResolution())
        , m_xyFaceResolution(c.xyMeshResolution())
    {
    }

    QMeshDataPtr operator ()() Q_DECL_OVERRIDE
    {
        return createCuboidMesh(m_xExtent, m_yExtent, m_zExtent,
                                m_yzFaceResolution,
                                m_xzFaceResolution,
                                m_xyFaceResolution);
    }

    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE
    {
        const CuboidMeshFunctor *otherFunctor = dynamic_cast<const CuboidMeshFunctor *>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_xExtent == m_xExtent &&
                    otherFunctor->m_yExtent == m_yExtent &&
                    otherFunctor->m_zExtent == m_zExtent &&
                    otherFunctor->m_yzFaceResolution == m_yzFaceResolution &&
                    otherFunctor->m_xzFaceResolution == m_xzFaceResolution &&
                    otherFunctor->m_xyFaceResolution == m_xyFaceResolution);
        return false;
    }

private:
    // Dimensions
    float m_xExtent;
    float m_yExtent;
    float m_zExtent;

    // Resolutions of faces with normal x, y, and z
    QSize m_yzFaceResolution;
    QSize m_xzFaceResolution;
    QSize m_xyFaceResolution;
};

QAbstractMeshFunctorPtr QCuboidMesh::meshFunctor() const
{
    return QAbstractMeshFunctorPtr(new CuboidMeshFunctor(*this));
}

enum PlaneNormal {
    PositiveX,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ
};

void generatePlaneData(float w, float h, const QSize &resolution,
                       PlaneNormal normal, float planeDistance,
                       float *vertices, quint16 *indices,
                       quint16 &baseVertex)
{
    const float a0 = -w / 2.0f;
    const float b0 = -h / 2.0f;
    const float da = w / (resolution.width() - 1);
    const float db = h / (resolution.height() - 1);
    const float du = 1.0 / (resolution.width() - 1);
    const float dv = 1.0 / (resolution.height() - 1);
    float n = 1.0f;

    switch (normal) {
    case NegativeX:
        n = -1.0f; // fall through
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
                *vertices++ = u;
                *vertices++ = v;

                // normal
                *vertices++ = n;
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
    }

    case NegativeY:
        n = -1.0f;
    case PositiveY: {
        // Iterate over z
        for (int j = 0; j < resolution.height(); ++j) {
            const float b = b0 + static_cast<float>(j) * db;
            const float v = static_cast<float>(j) * dv;

            // Iterate over x
            // This iterates in the opposite sense to the other directions
            // so that the winding order is correct
            for (int i = resolution.width() - 1; i >= 0; --i) {
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
                *vertices++ = n;
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
        n = -1.0f;
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
                *vertices++ = n;

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

    // Populate indices taking care to get correct CCW winding on all faces
    if (n > 0.0) {
        for (int j = 0; j < resolution.height() - 1; ++j) {
            const int rowStartIndex = j * resolution.width() + baseVertex;
            const int nextRowStartIndex = (j + 1) * resolution.width() + baseVertex;

            // Iterate over x
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
    } else {
        for (int j = 0; j < resolution.height() - 1; ++j) {
            const int rowStartIndex = j * resolution.width() + baseVertex;
            const int nextRowStartIndex = (j + 1) * resolution.width() + baseVertex;

            // Iterate over x
            for (int i = 0; i < resolution.width() - 1; ++i) {
                // Split quad into two triangles
                *indices++ = rowStartIndex + i;
                *indices++ = nextRowStartIndex + i;
                *indices++ = rowStartIndex + i + 1;

                *indices++ = nextRowStartIndex + i;
                *indices++ = nextRowStartIndex + i + 1;
                *indices++ = rowStartIndex + i + 1;
            }
        }
    }

    baseVertex += resolution.width() * resolution.height();
}

QMeshDataPtr createCuboidMesh(float xExtent,
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

    // Allocate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray vertexBytes;
    vertexBytes.resize(stride * nVerts);
    float* vertices = reinterpret_cast<float*>(vertexBytes.data());

    // Allocate a buffer for the index data
    const int yzIndices = 2 * 3 * (yzResolution.width() - 1) * (yzResolution.height() - 1);
    const int xzIndices = 2 * 3 * (xzResolution.width() - 1) * (xzResolution.height() - 1);
    const int xyIndices = 2 * 3 * (xyResolution.width() - 1) * (xyResolution.height() - 1);
    const int indexCount = 2 * (yzIndices + xzIndices + xyIndices);
    Q_ASSERT(indexCount < (std::numeric_limits<quint16>::max)());
    QByteArray indexBytes;
    indexBytes.resize(indexCount * sizeof(quint16));
    quint16* indices = reinterpret_cast<quint16*>(indexBytes.data());

    // Populate vertex and index buffers
    quint16 baseVertex = 0;
    generatePlaneData(yExtent, zExtent, yzResolution,
                      PositiveX, xExtent / 2.0f,
                      vertices, indices, baseVertex);

    vertices += yzVerts * elementSize;
    indices += yzIndices;
    generatePlaneData(yExtent, zExtent, yzResolution,
                      NegativeX, -xExtent / 2.0f,
                      vertices, indices, baseVertex);
    vertices += yzVerts * elementSize;
    indices += yzIndices;

    generatePlaneData(xExtent, zExtent, xzResolution,
                      PositiveY, yExtent / 2.0f,
                      vertices, indices, baseVertex);
    vertices += xzVerts * elementSize;
    indices += xzIndices;
    generatePlaneData(xExtent, zExtent, xzResolution,
                      NegativeY, -yExtent / 2.0f,
                      vertices, indices, baseVertex);
    vertices += xzVerts * elementSize;
    indices += xzIndices;

    generatePlaneData(xExtent, yExtent, xyResolution,
                      PositiveZ, zExtent / 2.0f,
                      vertices, indices, baseVertex);
    vertices += xyVerts * elementSize;
    indices += xyIndices;
    generatePlaneData(xExtent, yExtent, xyResolution,
                      NegativeZ, -zExtent / 2.0f,
                      vertices, indices, baseVertex);

    // Wrap the raw bytes in buffers and set attributes/indices on the mesh
    BufferPtr vertexBuffer(new Buffer(QOpenGLBuffer::VertexBuffer));
    vertexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    vertexBuffer->setData(vertexBytes);

    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);

    QMeshDataPtr mesh(new QMeshData(QMeshData::Triangles));
    quint32 offset = 0;
    mesh->addAttribute(QMeshData::defaultPositionAttributeName(),
                       AttributePtr(new Attribute(vertexBuffer, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += 3 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultTextureCoordinateAttributeName(),
                       AttributePtr(new Attribute(vertexBuffer, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += 2 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultNormalAttributeName(),
                       AttributePtr(new Attribute(vertexBuffer, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += 3 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultTangentAttributeName(),
                       AttributePtr(new Attribute(vertexBuffer, GL_FLOAT_VEC4, nVerts, offset, stride)));

    mesh->setIndexAttribute(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indexCount, 0, 0)));

    mesh->computeBoundsFromAttribute(QMeshData::defaultPositionAttributeName());
    qCDebug(Render::Frontend) << "computed axis-aligned bounding box is:" << mesh->boundingBox();

    return mesh;
}

} // namespace Qt3D

QT_END_NAMESPACE
