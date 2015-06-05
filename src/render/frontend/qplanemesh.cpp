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
#include "qplanemesh_p.h"

#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

#include <limits>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QPlaneMeshPrivate
    \internal
*/
QPlaneMeshPrivate::QPlaneMeshPrivate()
    : QAbstractMeshPrivate()
    , m_width(1.0f)
    , m_height(1.0f)
    , m_meshResolution(2, 2)
{
}

QPlaneMesh::QPlaneMesh(QNode *parent)
    : QAbstractMesh(*new QPlaneMeshPrivate, parent)
{
    update();
}

void QPlaneMesh::copy(const QNode *ref)
{
    QAbstractMesh::copy(ref);
    const QPlaneMesh *mesh = static_cast<const QPlaneMesh*>(ref);
    d_func()->m_width = mesh->d_func()->m_width;
    d_func()->m_height = mesh->d_func()->m_height;
    d_func()->m_meshResolution = mesh->d_func()->m_meshResolution;
}

/*! \internal */
QPlaneMesh::QPlaneMesh(QPlaneMeshPrivate &dd, QNode *parent)
    : QAbstractMesh(dd, parent)
{
    update();
}


void QPlaneMesh::setWidth(float width)
{
    Q_D(QPlaneMesh);
    if (width != d->m_width) {
        d->m_width = width;
        emit widthChanged();
        update();
    }
}

float QPlaneMesh::width() const
{
    Q_D(const QPlaneMesh);
    return d->m_width;
}

void QPlaneMesh::setHeight(float height)
{
    Q_D(QPlaneMesh);
    if (height != d->m_height) {
        d->m_height = height;
        emit heightChanged();
        update();
    }
}

float QPlaneMesh::height() const
{
    Q_D(const QPlaneMesh);
    return d->m_height;
}

void QPlaneMesh::setMeshResolution(const QSize &resolution)
{
    Q_D(QPlaneMesh);
    if (resolution != d->m_meshResolution) {
        d->m_meshResolution = resolution;
        emit meshResolutionChanged();
        update();
    }
}

QSize QPlaneMesh::meshResolution() const
{
    Q_D(const QPlaneMesh);
    return d->m_meshResolution;
}

QMeshDataPtr createPlaneMesh(float w, float h, const QSize &resolution);

class PlaneMeshFunctor : public QAbstractMeshFunctor
{
public:
    PlaneMeshFunctor(const QPlaneMesh &plane)
        : m_width(plane.width())
        , m_height(plane.height())
        , m_meshResolution(plane.meshResolution())
    {
    }

    QMeshDataPtr operator ()() Q_DECL_OVERRIDE
    {
        return createPlaneMesh(m_width, m_height, m_meshResolution);
    }

    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE
    {
        const PlaneMeshFunctor *otherFunctor = dynamic_cast<const PlaneMeshFunctor *>(&other);
        if (otherFunctor != Q_NULLPTR)
            return (otherFunctor->m_width == m_width &&
                    otherFunctor->m_height == m_height &&
                    otherFunctor->m_meshResolution == m_meshResolution);
        return false;
    }

private:
    float m_width;
    float m_height;
    QSize m_meshResolution;
};

QMeshDataPtr createPlaneMesh(float w, float h, const QSize &resolution)
{
    Q_ASSERT(w > 0.0f);
    Q_ASSERT(h > 0.0f);
    Q_ASSERT(resolution.width() >= 2);
    Q_ASSERT(resolution.height() >= 2);

    const int nVerts = resolution.width() * resolution.height();

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float x0 = -w / 2.0f;
    const float z0 = -h / 2.0f;
    const float dx = w / (resolution.width() - 1);
    const float dz = h / (resolution.height() - 1);
    const float du = 1.0 / (resolution.width() - 1);
    const float dv = 1.0 / (resolution.height() - 1);

    // Iterate over z
    for (int j = 0; j < resolution.height(); ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float v = static_cast<float>(j) * dv;

        // Iterate over x
        for (int i = 0; i < resolution.width(); ++i) {
            const float x = x0 + static_cast<float>(i) * dx;
            const float u = static_cast<float>(i) * du;

            // position
            *fptr++ = x;
            *fptr++ = 0.0;
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = v;

            // normal
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    // Wrap the raw bytes in a buffer
    BufferPtr buf(new Buffer(QOpenGLBuffer::VertexBuffer));
    buf->setUsage(QOpenGLBuffer::StaticDraw);
    buf->setData(bufferBytes);

    // Create the mesh data, specify the vertex format and data
    QMeshDataPtr mesh(new QMeshData(QMeshData::Triangles));
    quint32 offset = 0;
    mesh->addAttribute(QMeshData::defaultPositionAttributeName(),
                       AttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += 3 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultTextureCoordinateAttributeName(),
                       AttributePtr(new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride)));
    offset += 2 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultNormalAttributeName(),
                       AttributePtr(new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride)));
    offset += 3 * sizeof(float);

    mesh->addAttribute(QMeshData::defaultTangentAttributeName(),
                       AttributePtr(new Attribute(buf, GL_FLOAT_VEC4, nVerts, offset, stride)));

    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (resolution.width() - 1) * (resolution.height() - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint16>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < resolution.height() - 1; ++j) {
        const int rowStartIndex = j * resolution.width();
        const int nextRowStartIndex = (j + 1) * resolution.width();

        // Iterate over x
        for (int i = 0; i < resolution.width() - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i + 1;
            *indexPtr++ = rowStartIndex + i + 1;
        }
    }

    // Wrap the index bytes in a buffer
    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);

    // Specify index data on the mesh
    mesh->setIndexAttribute(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

    mesh->computeBoundsFromAttribute(QMeshData::defaultPositionAttributeName());
    qCDebug(Render::Frontend) << "computed axis-aligned bounding box is:" << mesh->boundingBox();

    return mesh;
}

QAbstractMeshFunctorPtr QPlaneMesh::meshFunctor() const
{
    return QAbstractMeshFunctorPtr(new PlaneMeshFunctor(*this));
}

} // namespace Qt3D

QT_END_NAMESPACE
